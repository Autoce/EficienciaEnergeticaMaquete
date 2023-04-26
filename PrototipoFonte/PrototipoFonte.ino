//corrigir os deletes da Area.cpp
// Tentar novamnete o if
// talvez iniciar as variaveis já como null
// um problema menor no area.cpp, mas tem que fazer de forma altomatica para o main


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <PubSubClient.h>
#include "Area.hpp"
#include "config.h"
#include "SPIFFS.h"

void areaInit();
void freeRTOSInit();
void SPIFFSInit();
void BlynkStartupTask(void* ptr);
void LEDUpdateTask(void* ptr);
void PIDUpdateTask(void* ptr);

void IRAM_ATTR doRead();
void timerConfig();

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
uint16_t PWR_LED = INIT_POWER;
double avgLx = 0;
int timerTime = UPDATE_TIME*1000;

Area *Area_0, *Area_1, *Area_2, *Area_3;
// LDR *LDR_1, *LDR_3;
// LED *LED_1, *LED_3;
SemaphoreHandle_t mutexArea, modeChangeSafety;

hw_timer_t *readTimer = NULL;
bool readSensor = false;

//MQTT
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//Light mode
size_t lightMode = 0;

void setup()
{
  Serial.begin(115200);
  Serial.print("[I] Prototype starting up...\n");
  disableCore0WDT();
  
  SPIFFSInit();
  areaInit(false, true, false, true);
  freeRTOSInit();

  timerConfig();

  // Configura o servidor MQTT e o cliente MQTT
  mqttClient.setServer(MQTT_BROKER_IP_ADDRESS, 1883);
  mqttClient.setCallback(mqttCallback);
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
  PWR_LED = round(param.asInt());
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
      // Verifica se a conexão com o broker MQTT está ativa e reconecta-se, se necessário
      if (!mqttClient.connected()) {
        reconnect();
      }

      // Processa as mensagens MQTT recebidas
      mqttClient.loop();


      AreaInfoBundle_t tmp;
      AreaInfo_t Info_0, Info_1, Info_2, Info_3;

      switch (lightMode)
      {
        case 0:
          tmp = Area_0->getInformation();
          Info_0 = tmp.a0;

          tmp = Area_2->getInformation();
          Info_2 = tmp.a0;

          tmp = Area_1->getInformation();
          Info_1 = tmp.a0;
          Info_3 = tmp.a1;
          
          break;

        case 1:
          tmp = Area_0->getInformation();
          Info_0 = tmp.a0;

          tmp = Area_1->getInformation();
          Info_1 = tmp.a0;

          tmp = Area_2->getInformation();
          Info_2= tmp.a0;

          tmp = Area_3->getInformation();
          Info_3 = tmp.a0;

          break;

        case 2:
          tmp = Area_0->getInformation();
          Info_0 = tmp.a0;
          Info_1 = tmp.a1;

          tmp = Area_1->getInformation();
          Info_2 = tmp.a0;
          Info_3 = tmp.a1;

          break;

        case 3:
          tmp = Area_0->getInformation();
          Info_0 = tmp.a0;
          Info_1 = tmp.a1;
          Info_2 = tmp.a2;
          Info_3 = tmp.a3;

          break;
        
        default:
          break;
      }
      
      Blynk.virtualWrite(V1, Info_0.dutyCycle);
      Blynk.virtualWrite(V2, Info_1.dutyCycle);
      Blynk.virtualWrite(V3, Info_2.dutyCycle);
      Blynk.virtualWrite(V4, Info_3.dutyCycle);
      Blynk.virtualWrite(V6, Info_0.Lx);
      Blynk.virtualWrite(V7, Info_2.Lx);
      Blynk.virtualWrite(V8, avgLx);

      Blynk.run();  

      
      // Publica uma mensagem MQTT
      char strValue[20];
      sprintf(strValue, "%.2f", Info_0.dutyCycle);
      mqttClient.publish("esp32/v1", strValue);
      sprintf(strValue, "%.2f", Info_1.dutyCycle);
      mqttClient.publish("esp32/v2", strValue);
      sprintf(strValue, "%.2f", Info_2.dutyCycle);
      mqttClient.publish("esp32/v3", strValue);
      sprintf(strValue, "%.2f", Info_3.dutyCycle);
      mqttClient.publish("esp32/v4", strValue);
      sprintf(strValue, "%.2f", Info_0.Lx);
      mqttClient.publish("esp32/v6", strValue);
      sprintf(strValue, "%.2f", Info_1.Lx);
      mqttClient.publish("esp32/v7", strValue);
      sprintf(strValue, "%.2f", Info_2.Lx);
      mqttClient.publish("esp32/v8", strValue);
      sprintf(strValue, "%.2f", Info_3.Lx);
      mqttClient.publish("esp32/v9", strValue);

      xSemaphoreGive(mutexArea);
    }
  }
}

void LEDUpdate(){
  if(readSensor && xSemaphoreTake(modeChangeSafety, 0)){
    switch (lightMode)
    {
      case 0:
        Area_0->update(LUX_REFERENCE);
        Area_1->update(PWR_LED);
        Area_2->update(LUX_REFERENCE);          
        break;

      case 1:
        Area_0->update(LUX_REFERENCE);
        Area_1->update(LUX_REFERENCE);
        Area_2->update(LUX_REFERENCE);
        Area_3->update(LUX_REFERENCE);
        break;

      case 2:
        Area_0->update(LUX_REFERENCE);
        Area_1->update(LUX_REFERENCE);
        break;

      case 3:
        Area_0->update(LUX_REFERENCE);
        break;
      
      default:
        break;
    }
    readSensor = false;
    xSemaphoreGive(modeChangeSafety);
  }
}

void areaInit(bool mode0, bool mode1, bool mode2, bool mode3){
  Serial.print("[I] Configurating areas");
  Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, AMOSTRAS_MED, LDR_POLY_COEFF[0], Kp, Ki, Kd, N, Ts, false);
  Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, AMOSTRAS_MED, LDR_POLY_COEFF[2], Kp, Ki, Kd, N, Ts, false);
  Area_1 = new Area(LDR1_PIN, LDR3_PIN, LED1_PIN, LED3_PIN, 1, 3, AMOSTRAS_MED, LDR_POLY_COEFF[1], LDR_POLY_COEFF[3], Kp, Ki, Kd, N, Ts, true);

  Serial.print(" [OK]\n");
}

void freeRTOSInit(){
  Serial.print("[I] Starting up FreeRTOS");
  mutexArea = xSemaphoreCreateMutex();
  modeChangeSafety = xSemaphoreCreateMutex();
  
  // xTaskCreatePinnedToCore(PIDUpdateTask, "PIDUpdt", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(BlynkStartupTask, "BlynkSTT", 4096, NULL, 1, NULL, 1); 
  Serial.print(" [OK]\n");
}

void SPIFFSInit(){
  Serial.print("[I] Starting up SPIFFS");
  fileSystemAvailable = SPIFFS.begin();
  if(!fileSystemAvailable)
  {
    Serial.print(" [FAILED]\n");
    Serial.print("[I] Formatting Flash ROM with SPIFFS");
    fileSystemAvailable = SPIFFS.begin(true);
    if(fileSystemAvailable) Serial.print(" [OK]\n");
    else  {
      Serial.print(" [FAILED]\n");
      Serial.print("[?] SPIFFS Initialization failed. Configurations will not be saved.\n");
    }
  }
  else Serial.print(" [OK]\n");
}

void timerConfig(){
  readTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(readTimer, &doRead, true);
  timerAlarmWrite(readTimer, timerTime, true);
  timerAlarmEnable(readTimer);
}

void IRAM_ATTR doRead(){
  readSensor = true;
}

//Muda o modo de operação
void changeMode(int mode){
  if(xSemaphoreTake(modeChangeSafety, portMAX_DELAY)){
    Serial.printf("Mudando para o modo %i\n", mode);
    switch(lightMode){
      case 0:
        delete Area_0, Area_1, Area_2;
        break;
      case 1:
        delete Area_0, Area_1, Area_2, Area_3;
        break;
      case 2:
        delete Area_0, Area_1;
        break;
      case 3:
        delete Area_0;
        break;
      default:
        break;
    }    
    switch (mode)
    {
    case 0:
      lightMode = 0;
      Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, AMOSTRAS_MED, LDR_POLY_COEFF[0], Kp, Ki, Kd, N, Ts, false);
      Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, AMOSTRAS_MED, LDR_POLY_COEFF[2], Kp, Ki, Kd, N, Ts, false);
      Area_1 = new Area(LDR1_PIN, LDR3_PIN, LED1_PIN, LED3_PIN, 1, 3, AMOSTRAS_MED, LDR_POLY_COEFF[1], LDR_POLY_COEFF[3], Kp, Ki, Kd, N, Ts, true);
      break;

    case 1:
      lightMode = 1;
      Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, AMOSTRAS_MED, LDR_POLY_COEFF[0], Kp, Ki, Kd, N, Ts, false);
      Area_1 = new Area(LDR1_PIN, LED1_PIN, 1, AMOSTRAS_MED, LDR_POLY_COEFF[1], Kp, Ki, Kd, N, Ts, false);
      Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, AMOSTRAS_MED, LDR_POLY_COEFF[2], Kp, Ki, Kd, N, Ts, false);
      Area_3 = new Area(LDR3_PIN, LED3_PIN, 3, AMOSTRAS_MED, LDR_POLY_COEFF[3], Kp, Ki, Kd, N, Ts, false);
      break;

    case 2:
      lightMode = 2;
      Area_0 = new Area(LDR0_PIN, LDR1_PIN, LED0_PIN, LED1_PIN, 0, 1, AMOSTRAS_MED, LDR_POLY_COEFF[0], LDR_POLY_COEFF[1], Kp, Ki, Kd, N, Ts, false);
      Area_1 = new Area(LDR2_PIN, LDR3_PIN, LED2_PIN, LED3_PIN, 2, 3, AMOSTRAS_MED, LDR_POLY_COEFF[2], LDR_POLY_COEFF[3], Kp, Ki, Kd, N, Ts, false);
      break;

    case 3:
      lightMode = 3;
      Area_0 = new Area(LDR0_PIN, LDR1_PIN, LDR2_PIN, LDR3_PIN, LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN, 0, 1, 2, 3, AMOSTRAS_MED, LDR_POLY_COEFF[0], LDR_POLY_COEFF[1], LDR_POLY_COEFF[2], LDR_POLY_COEFF[3], Kp, Ki, Kd, N, Ts, false);
    
    default:
      break;
    }
    xSemaphoreGive(modeChangeSafety);
  }  
}

//MQTT
// Função de reconexão ao broker MQTT
void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Tentando reconectar ao broker MQTT...");
    if (mqttClient.connect("ESP32Client", MQTTUSERNAME, MQTTPWD)) {
      Serial.println("Reconectado ao broker MQTT!");
      mqttClient.subscribe("ha/mode");
      mqttClient.subscribe("ha/lux");
      mqttClient.subscribe("ha/pwr");
    } else {
      Serial.print("Falha ao se reconectar ao broker MQTT com erro: ");
      Serial.println(mqttClient.state());
    }
  }
}

// Função de callback para processar as mensagens MQTT recebidas
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String message = (char*)payload;

  if(strcmp(topic, "ha/mode") == 0){
    changeMode(message.toInt());
  }
  else if(strcmp(topic, "ha/lux")==0){
    LUX_REFERENCE = message.toFloat();
    Serial.printf("[I] Luminance reference changed, using MQTT. New reference: %.2f lx\n", LUX_REFERENCE);
  }
  else if(strcmp(topic, "ha/pwr")==0){
    PWR_LED = round(message.toInt());
    Serial.printf("[I] LED power configuration changed, using MQTT. New power: %i\n", PWR_LED);
  }

  Serial.print("Mensagem MQTT recebida no topico [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
