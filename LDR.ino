#include <PID_v1.h>

#define LDR0_PIN 34
#define LDR1_PIN 35
#define LDR2_PIN 32
#define LDR3_PIN 33
#define LED0_PIN 19
#define LED1_PIN 21
#define LED2_PIN 22
#define LED3_PIN 23
#define LDR_TEST 3
#define AMOSTRAS_MED 200

typedef struct
{
  uint8_t ptr;
  float values[AMOSTRAS_MED];
} circular_array;

const float  LDR_POLY_COEFF[][8] =
{
  {3.68084919177402, -34.5120701709678, 131.080431022415,  -249.864706948331, 255.443445134104, -124.166605221456, 43.7549060222041, -1.19323952996493},
  {1.93957819560206, -15.4571559304617,  56.8361457426944, -115.665743300122, 150.58478018297,  -88.0892324785287, 74.0271437691636, -0.44040656912988},
  {4.82454368495804, -46.0248426386745, 176.9707408102,    -340.759585065101, 349.717590955888, -169.928843142655, 56.8947926737471, -1.68124423595064},
  {6.0378747625652,  -66.5375903505571, 293.002869190919,  -647.669495955518, 750.201322448095, -424.435876089206, 103.42736436519,  -5.43184919105217}
};
const uint8_t LDR_INPUT[] = {LDR0_PIN, LDR1_PIN, LDR2_PIN, LDR3_PIN};
const uint8_t LED_CONTROL[] = {LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN};
circular_array LDR_array[4];

float ADCtoLx(uint16_t ADC, uint8_t LDR);
void initializeLDR();
void sampleData();
float readLDR(uint8_t LDR);

void setup() 
{
  Serial.begin(115200);
  for(uint8_t i = 0; i < 4; i++) 
  {
    pinMode(LDR_INPUT[i], OUTPUT);
    ledcSetup(i, 10000, 10);
    ledcAttachPin(LED_CONTROL[i], i);
    ledcWrite(i, 1023);
  }
  initializeLDR();
  for(int i = 0; i < AMOSTRAS_MED; i++)
  {
    Serial.println(LDR_array[0].values[i]);
  }
}

void loop() 
{
  sampleData();
  Serial.println(String(readLDR(0)) + "\t" + String(readLDR(1)) + "\t" + String(readLDR(2)) + "\t" + String(readLDR(3)));
  delayMicroseconds(50);
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
    sampleData();
    delayMicroseconds(50);
  }
}

void sampleData()
{
  float Lx[4];
  for(uint8_t i=0; i<4; i++) 
  {
    uint16_t ADC[4];
    ADC[i] = analogRead(LDR_INPUT[i]);
    Lx[i] = ADCtoLx(ADC[i], i);
    insert(Lx[i], &LDR_array[i]);
  }
}

float readLDR(uint8_t LDR)
{
  float sum = 0;
  for(uint8_t l=0; l<AMOSTRAS_MED; l++) sum += LDR_array[LDR].values[l];
  return sum/AMOSTRAS_MED;
}
