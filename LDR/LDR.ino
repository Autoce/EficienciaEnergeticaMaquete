#include "PID.hpp"
#include <WiFi.h>
#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID "TMPLfD1ibBnK"
#define BLYNK_DEVICE_NAME "LDRSENSORS"
#define BLYNK_AUTH_TOKEN "ItbW7xImWPzsB9nzaV7wjWH42NVoI6Yy"
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>

#define LDR0_PIN 33
#define LDR1_PIN 32
#define LDR2_PIN 35
#define LDR3_PIN 34


#define LED0_PIN 19
#define LED1_PIN 21
#define LED2_PIN 22
#define LED3_PIN 23

#define AMOSTRAS_MED 100
#define STARTUP_REF 300

typedef struct
{
  uint8_t ptr;
  float values[AMOSTRAS_MED];
} circular_array;

const float LDR_POLY_COEFF[][8] =
{
  {3.68084919177402, -34.5120701709678, 131.080431022415,  -249.864706948331, 255.443445134104, -124.166605221456, 43.7549060222041, -1.19323952996493},
  {1.93957819560206, -15.4571559304617,  56.8361457426944, -115.665743300122, 150.58478018297,  -88.0892324785287, 74.0271437691636, -0.44040656912988},
  {4.82454368495804, -46.0248426386745, 176.9707408102,    -340.759585065101, 349.717590955888, -169.928843142655, 56.8947926737471, -1.68124423595064},
  {6.0378747625652,  -66.5375903505571, 293.002869190919,  -647.669495955518, 750.201322448095, -424.435876089206, 103.42736436519,  -5.43184919105217}
};

const uint8_t LDR_INPUT[] = {LDR0_PIN, LDR1_PIN, LDR2_PIN, LDR3_PIN};
const uint8_t LED_CONTROL[] = {LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN};

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "LARM_ALUNOS";
char pass[] = "LarmUfscq2022";

double LUX_REFERENCE[4];
double LDR_FILTERED[4];
double PWM_OUTPUT[4];

const double Kp = 1, Ki = 5, Kd = 0;
unsigned long time_past = 0;

PID* PID_SYS[4];
circular_array LDR_array[4];

float ADCtoLx(uint16_t ADC, uint8_t LDR);
void initializeLDR();
void sampleLDR();

void setup() 
{
  Serial.begin(115200);
  Serial.print("Iniciando Blynk...");
  Blynk.begin(auth, ssid, pass);
  disableCore0WDT();
  disableCore1WDT();
  for(uint8_t i = 0; i < 4; i++) 
  {
    PWM_OUTPUT[i] = 0;
    LUX_REFERENCE[i] = STARTUP_REF;
    PID_SYS[i] = new PID(&LDR_FILTERED[i], &PWM_OUTPUT[i], &LUX_REFERENCE[i], Kp, Ki, Kd, 2, 0.001);
    pinMode(LDR_INPUT[i], OUTPUT);
    ledcSetup(i, 10000, 10);
    ledcAttachPin(LED_CONTROL[i], i);
    ledcWrite(i, PWM_OUTPUT[i]);
  }
  initializeLDR();
  xTaskCreatePinnedToCore(Control_Logic, "Control_Logic", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Lux_Update, "Lux_Update", 4096, NULL, 1, NULL, 1);
  vTaskStartScheduler();
  Blynk.virtualWrite(V0, STARTUP_REF);
  Blynk.run();
}
void Lux_Update(void* ptr)
{
  while(1)
  {
    Serial.printf("%f - %f - %f - %f\n", LDR_FILTERED[0], LDR_FILTERED[1], LDR_FILTERED[2], LDR_FILTERED[3]);
  }
}
void loop() 
{
  Blynk.virtualWrite(V1, PWM_OUTPUT[0]/10.23);
  Blynk.virtualWrite(V2, PWM_OUTPUT[1]/10.23);
  Blynk.virtualWrite(V3, PWM_OUTPUT[2]/10.23);
  Blynk.virtualWrite(V4, PWM_OUTPUT[3]/10.23);
  Blynk.run();
}
BLYNK_WRITE(V0)
{
  float a = param.asFloat();
  LUX_REFERENCE[0] = a;
  LUX_REFERENCE[1] = a;
  LUX_REFERENCE[2] = a;
  LUX_REFERENCE[3] = a;
}
void Control_Logic(void* ptr)
{
  while(1)
  {
    unsigned long now = millis();
    if(now > time_past + 1)
    {
      sampleLDR();
      for(uint8_t i = 0; i<4; i++) 
      {
        PID_SYS[i]->Compute();
        ledcWrite(i, PWM_OUTPUT[i]);
      }
      time_past = now;
    }
  }
}
float ADCtoLx(uint16_t ADC, uint8_t LDR)
{
  float Lx = 0;
  float scaledADC = ADC/1000.0;
  for(uint8_t i = 0; i < 8; i++)
    Lx += LDR_POLY_COEFF[LDR][i]*pow(scaledADC, 7-i);
  if(Lx < 0) return 0;
  return Lx;
}

void insert(float val, circular_array* arr)
{
  arr->values[arr->ptr] = val;
  arr->ptr = (arr->ptr+1)%AMOSTRAS_MED;
}

void initializeLDR()
{
  for(uint8_t l=0; l<4; l++) LDR_array[l].ptr = 0;
  for(uint8_t l=0; l<AMOSTRAS_MED; l++)
  {
    sampleLDR();
    delay(1);
  }
}
void sampleLDR()
{
  float Lx[4];
  for(uint8_t i=0; i<4; i++) 
  {
    uint16_t ADC[4];
    ADC[i] = analogRead(LDR_INPUT[i]);
    Lx[i] = ADCtoLx(ADC[i], i);
    insert(Lx[i], &LDR_array[i]);
    float sum = 0;
    for(uint8_t l=0; l<AMOSTRAS_MED; l++) sum += LDR_array[i].values[l];
    LDR_FILTERED[i] = sum/AMOSTRAS_MED;
  }
}
