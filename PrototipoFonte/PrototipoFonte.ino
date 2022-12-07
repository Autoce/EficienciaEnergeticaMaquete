#include "Area.hpp"
#include "config.h"
#include <WiFi.h>
#include <WiFiClient.h>

#define BLYNK_TEMPLATE_ID "TMPLfD1ibBnK"
#define BLYNK_DEVICE_NAME "LDRSENSORS"
#define BLYNK_AUTH_TOKEN "ItbW7xImWPzsB9nzaV7wjWH42NVoI6Yy"
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>

const double LDR_POLY_COEFF[][8] =
{
  {3.68084919177402, -34.5120701709678, 131.080431022415,  -249.864706948331, 255.443445134104, -124.166605221456, 43.7549060222041, -1.19323952996493},
  {1.93957819560206, -15.4571559304617,  56.8361457426944, -115.665743300122, 150.58478018297,  -88.0892324785287, 74.0271437691636, -0.44040656912988},
  {4.82454368495804, -46.0248426386745, 176.9707408102,    -340.759585065101, 349.717590955888, -169.928843142655, 56.8947926737471, -1.68124423595064},
  {6.0378747625652,  -66.5375903505571, 293.002869190919,  -647.669495955518, 750.201322448095, -424.435876089206, 103.42736436519,  -5.43184919105217}
};

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "LARM_ALUNOS";
char pass[] = "LarmUfscq2022";

const double Kp = 1, Ki = 5, Kd = 0, N = 1, Ts = 0.001;
double LUX_REFERENCE = STARTUP_REF;
double avgLx = 0;
unsigned long time_past = 0;

AreaInfo_t Info_0, Info_2;
Area *Area_0, *Area_2;
LDR *LDR_1, *LDR_3;
LED *LED_1, *LED_3;
SemaphoreHandle_t mutexArea;

hw_timer_t *ctrlTimer;

void setup()
{
  disableCore0WDT();
  
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, STARTUP_REF);
  Blynk.virtualWrite(V5, 0);
  Blynk.run();
  
  Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, LDR_POLY_COEFF[0], Kp, Ki, Kd, N, Ts);
  Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, LDR_POLY_COEFF[2], Kp, Ki, Kd, N, Ts);
  Info_0 = Area_0->getInformation();
  Info_2 = Area_2->getInformation();

  LDR_1 = new LDR(LDR1_PIN, LDR_POLY_COEFF[1]);
  LDR_3 = new LDR(LDR3_PIN, LDR_POLY_COEFF[3]);

  LED_1 = new LED(LED1_PIN, 1);
  LED_3 = new LED(LED3_PIN, 3);
  
  mutexArea = xSemaphoreCreateMutex();
  
  xTaskCreatePinnedToCore(PIDUpdateTask, "PIDUpdt", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(LEDUpdateTask, "LEDUpdt", 1512, NULL, 1, NULL, 1); 
}

void loop()
{
  if(xSemaphoreTake(mutexArea, 0))
  {
    Info_0 = Area_0->getInformation();
    Info_2 = Area_2->getInformation();
    avgLx = (LDR_1->getLuminance() + LDR_3->getLuminance())*0.5;
    
    xSemaphoreGive(mutexArea);
    
    Blynk.virtualWrite(V1, Info_0.dutyCycle);
    Blynk.virtualWrite(V2, LED_1->getDutyCycle());
    Blynk.virtualWrite(V3, Info_2.dutyCycle);
    Blynk.virtualWrite(V4, LED_3->getDutyCycle());
  
    Blynk.virtualWrite(V6, Info_0.Lx);
    Blynk.virtualWrite(V7, Info_2.Lx);
    Blynk.virtualWrite(V8, avgLx);
  
    Blynk.run();
    
    Serial.printf("Area 0: %.2f lx - %.2f%% == Area 2: %.2f lx - %.2f%%\n", Info_0.Lx, Info_0.dutyCycle, Info_2.Lx, Info_2.dutyCycle);
    
  }  
}

BLYNK_WRITE(V0)
{
  LUX_REFERENCE = param.asFloat();
  Serial.printf("Luminance reference changed. New reference: %.2f lx\n", LUX_REFERENCE);
}

BLYNK_WRITE(V5)
{
  uint16_t PWR_LED = round(param.asInt()*10.23);
  LED_1->analogWrite(PWR_LED);
  LED_3->analogWrite(PWR_LED);
  Serial.printf("LED power configuration changed. New power: %i\n", PWR_LED);
}

void LEDUpdateTask(void* ptr)
{
  while(1)
  {
    LDR_1->update();
    LDR_3->update();
  }
}
void PIDUpdateTask(void* ptr)
{
  while(1)
  {
    if(xSemaphoreTake(mutexArea, 0))
    {
      Area_0->update(LUX_REFERENCE);
      Area_2->update(LUX_REFERENCE);
      xSemaphoreGive(mutexArea);
    }
  }
}
