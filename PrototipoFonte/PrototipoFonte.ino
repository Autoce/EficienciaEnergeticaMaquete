#include "Area.hpp"
#include "config.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "SPIFFS.h"

void areaInit();
void freeRTOSInit();
void SPIFFSInit();
void BlynkStartupTask(void* ptr);
void LEDUpdateTask(void* ptr);
void PIDUpdateTask(void* ptr);

const double LDR_POLY_COEFF[][8] =
{
  {3.68084919177402, -34.5120701709678, 131.080431022415,  -249.864706948331, 255.443445134104, -124.166605221456, 43.7549060222041, -1.19323952996493},
  {1.93957819560206, -15.4571559304617,  56.8361457426944, -115.665743300122, 150.58478018297,  -88.0892324785287, 74.0271437691636, -0.44040656912988},
  {4.82454368495804, -46.0248426386745, 176.9707408102,    -340.759585065101, 349.717590955888, -169.928843142655, 56.8947926737471, -1.68124423595064},
  {6.0378747625652,  -66.5375903505571, 293.002869190919,  -647.669495955518, 750.201322448095, -424.435876089206, 103.42736436519,  -5.43184919105217}
};

bool fileSystemAvailable = false;
const double Kp = INIT_KP, Ki = INIT_KI, Kd = INIT_KD, N = 1, Ts = INIT_SAMPLING_TIME;

double LUX_REFERENCE = INIT_REFERENCE;
double avgLx = 0;

AreaInfo_t Info_0, Info_2;
Area *Area_0, *Area_2;
LDR *LDR_1, *LDR_3;
LED *LED_1, *LED_3;
SemaphoreHandle_t mutexArea;

void setup()
{
  Serial.begin(115200);
  Serial.print("[I] Prototype starting up...\n");
  disableCore0WDT();
  
  SPIFFSInit();
  areaInit();
  freeRTOSInit();
}

void loop()
{
  LEDUpdate();  
}

BLYNK_WRITE(V0)
{
  LUX_REFERENCE = param.asFloat();
  Serial.printf("[I] Luminance reference changed. New reference: %.2f lx\n", LUX_REFERENCE);
}

BLYNK_WRITE(V5)
{
  uint16_t PWR_LED = round(param.asInt()*10.23);
  LED_1->analogWrite(PWR_LED);
  LED_3->analogWrite(PWR_LED);
  Serial.printf("[I] LED power configuration changed. New power: %i\n", PWR_LED);
}

void BlynkStartupTask(void* ptr)
{
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, LUX_REFERENCE);
  Blynk.virtualWrite(V5, 0);
  Blynk.run();
  while(1)
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
    }
  }
}

void LEDUpdate()
{
    LDR_1->update();
    LDR_3->update();
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

void areaInit()
{
  Serial.print("[I] Configurating areas");
  Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, AMOSTRAS_MED, LDR_POLY_COEFF[0], Kp, Ki, Kd, N, Ts);
  Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, AMOSTRAS_MED, LDR_POLY_COEFF[2], Kp, Ki, Kd, N, Ts);
  Info_0 = Area_0->getInformation();
  Info_2 = Area_2->getInformation();

  LDR_1 = new LDR(LDR1_PIN, AMOSTRAS_MED, LDR_POLY_COEFF[1]);
  LDR_3 = new LDR(LDR3_PIN, AMOSTRAS_MED, LDR_POLY_COEFF[3]);

  LED_1 = new LED(LED1_PIN, 1);
  LED_3 = new LED(LED3_PIN, 3);

  LED_1->analogWrite(0);
  LED_3->analogWrite(0);
  Serial.print(" [OK]\n");
}

void freeRTOSInit()
{
  Serial.print("[I] Starting up FreeRTOS");
  mutexArea = xSemaphoreCreateMutex();
  
  xTaskCreatePinnedToCore(PIDUpdateTask, "PIDUpdt", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(BlynkStartupTask, "BlynkSTT", 4096, NULL, 1, NULL, 1); 
  Serial.print(" [OK]\n");
}

void SPIFFSInit()
{
  Serial.print("[I] Starting up SPIFFS");
  fileSystemAvailable = SPIFFS.begin();
  if(!fileSystemAvailable)
  {
    Serial.print(" [FAILED]\n");
    Serial.print("[I] Formatting Flash ROM with SPIFFS");
    fileSystemAvailable = SPIFFS.begin(true);
    if(fileSystemAvailable) Serial.print(" [OK]\n");
    else
    {
      Serial.print(" [FAILED]\n");
      Serial.print("[?] SPIFFS Initialization failed. Configurations will not be saved.\n");
    }
  }
  else Serial.print(" [OK]\n");
}

//Serial.printf("Area 0: %.2f lx - %.2f%% == Area 2: %.2f lx - %.2f%%\n", Info_0.Lx, Info_0.dutyCycle, Info_2.Lx, Info_2.dutyCycle);
