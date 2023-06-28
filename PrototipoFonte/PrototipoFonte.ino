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

// const double LDR_POLY_COEFF[4][102] =
// {
//   {0, 777, 1170, 1393, 1564, 1696, 1804, 1899, 1974, 2050, 2109, 2169, 2225, 2272, 2314, 2357, 2397, 2434, 2472, 2500, 2533, 2563, 2602, 2623, 2640, 2671, 2696, 2711, 2734, 2757, 2767, 2790, 2804, 2826, 2838, 2853, 2877, 2892, 2893, 2916, 2920, 2945, 2954, 2965, 2979, 2973, 3003, 3003, 3011, 3014, 3032, 3029, 3047, 3047, 3057, 3063, 3073, 3082, 3089, 3099, 3108, 3112, 3123, 3135, 3139, 3144, 3154, 3166, 3173, 3177, 3187, 3190, 3204, 3208, 3215, 3223, 3232, 3237, 3252, 3256, 3256, 3269, 3268, 3271, 3277, 3293, 3285, 3303, 3301, 3313, 3316, 3308, 3324, 3326, 3347, 3340, 3332, 3347, 3348, 3343, 3344, 4095},
//   {0, 361, 657, 847, 995, 1134, 1239, 1337, 1418, 1502, 1578, 1648, 1707, 1764, 1819, 1864, 1912, 1958, 2004, 2040, 2077, 2115, 2153, 2178, 2208, 2241, 2263, 2295, 2317, 2340, 2361, 2385, 2406, 2422, 2449, 2465, 2490, 2513, 2516, 2543, 2555, 2577, 2595, 2618, 2633, 2640, 2663, 2677, 2691, 2695, 2716, 2718, 2734, 2741, 2760, 2763, 2773, 2784, 2799, 2803, 2815, 2830, 2835, 2847, 2859, 2867, 2877, 2879, 2894, 2898, 2904, 2920, 2921, 2930, 2940, 2941, 2950, 2958, 2965, 2966, 2975, 2982, 2988, 2995, 3002, 3009, 3012, 3022, 3027, 3032, 3039, 3044, 3057, 3060, 3064, 3071, 3088, 3092, 3091, 3095, 3091, 4095},
//   {0, 658, 1020, 1237, 1395, 1535, 1646, 1735, 1818, 1901, 1960, 2023, 2084, 2134, 2190, 2230, 2274, 2313, 2356, 2384, 2412, 2449, 2491, 2504, 2526, 2557, 2576, 2602, 2628, 2641, 2668, 2678, 2698, 2716, 2733, 2748, 2766, 2781, 2796, 2815, 2825, 2840, 2855, 2867, 2883, 2894, 2907, 2923, 2933, 2938, 2955, 2967, 2974, 2981, 2997, 3002, 3006, 3018, 3028, 3035, 3041, 3054, 3059, 3069, 3081, 3089, 3098, 3103, 3112, 3119, 3124, 3135, 3139, 3147, 3154, 3159, 3167, 3172, 3180, 3186, 3192, 3196, 3203, 3210, 3218, 3223, 3227, 3235, 3242, 3251, 3254, 3256, 3265, 3271, 3274, 3282, 3289, 3294, 3296, 3301, 3299, 4095},
//   {0, 1502, 1910, 2124, 2270, 2383, 2471, 2540, 2613, 2688, 2718, 2766, 2805, 2841, 2888, 2913, 2948, 2974, 3000, 3014, 3035, 3060, 3093, 3106, 3129, 3141, 3159, 3174, 3190, 3209, 3220, 3239, 3253, 3262, 3285, 3292, 3308, 3314, 3324, 3336, 3344, 3359, 3368, 3381, 3392, 3396, 3405, 3418, 3427, 3433, 3446, 3451, 3464, 3468, 3485, 3479, 3485, 3491, 3503, 3511, 3512, 3520, 3528, 3532, 3540, 3545, 3552, 3561, 3565, 3570, 3576, 3583, 3591, 3597, 3603, 3608, 3613, 3624, 3637, 3633, 3643, 3653, 3648, 3661, 3667, 3670, 3682, 3672, 3687, 3687, 3695, 3689, 3701, 3692, 3698, 3702, 3710, 3715, 3722, 3723, 3720, 4095}
// };

const double Lux_Reference[4][102] = 
{
  {0, 6, 38, 53, 66, 80, 92, 104, 116, 129, 140, 152, 164, 177, 188, 126, 193, 225, 236, 248, 260, 273, 284, 295, 307, 320, 332, 342, 355, 367, 379, 391, 403, 415, 428, 439, 451, 463, 476, 487, 499, 511, 524, 536, 548, 561, 573, 585, 597, 608, 621, 633, 645, 657, 669, 682, 694, 706, 718, 731, 743, 755, 766, 779, 791, 803, 815, 828, 840, 852, 865, 878, 890, 902, 914, 926, 938, 951, 962, 975, 988, 1000, 1012, 1025, 1038, 1050, 1062, 1074, 1087, 1099, 1111, 1124, 1137, 1149, 1162, 1174, 1188, 1200, 1211, 1214, 1215, 6000},
  {0, 4, 34, 46, 57, 67, 76, 85, 96, 106, 115, 123, 132, 142, 151, 84, 169, 181, 193, 207, 220, 233, 243, 255, 267, 280, 292, 303, 315, 327, 340, 351, 343, 376, 388, 400, 411, 423, 436, 447, 459, 471, 484, 496, 508, 520, 533, 545, 556, 568, 580, 592, 604, 616, 627, 640, 652, 664, 676, 689, 701, 712, 724, 737, 748, 760, 772, 784, 797, 808, 821, 834, 846, 857, 870, 882, 894, 906, 917, 929, 942, 954, 966, 978, 992, 1004, 1016, 1028, 1040, 1052, 1065, 1076, 1089, 1101, 1113, 1125, 1138, 1151, 1163, 1168, 1171, 1215},
  {0, 4, 34, 46, 57, 67, 76, 85, 96, 106, 115, 123, 132, 142, 151, 84, 169, 181, 193, 207, 220, 233, 243, 255, 267, 280, 292, 303, 315, 327, 340, 351, 343, 376, 388, 400, 411, 423, 436, 447, 459, 471, 484, 496, 508, 520, 533, 545, 556, 568, 580, 592, 604, 616, 627, 640, 652, 664, 676, 689, 701, 712, 724, 737, 748, 760, 772, 784, 797, 808, 821, 834, 846, 857, 870, 882, 894, 906, 917, 929, 942, 954, 966, 978, 992, 1004, 1016, 1028, 1040, 1052, 1065, 1076, 1089, 1101, 1113, 1125, 1138, 1151, 1163, 1168, 1171, 1215},
  {0, 5, 38, 52, 65, 77, 89, 101, 113, 126, 137, 149, 160, 173, 185, 197, 108, 220, 232, 243, 254, 267, 279, 291, 302, 315, 326, 337, 349, 360, 373, 385, 396, 407, 420, 432, 444, 455, 468, 479, 491, 502, 515, 526, 538, 549, 562, 574, 586, 597, 610, 622, 633, 643, 655, 667, 679, 690, 702, 714, 726, 737, 749, 762, 773, 785, 795, 808, 819, 831, 843, 856, 867, 878, 890, 903, 915, 926, 938, 948, 961, 972, 984, 996, 1009, 1020, 1032, 1044, 1056, 1068, 1079, 1091, 1103, 1113, 1125, 1137, 1150, 1162, 1174, 1178, 1179, 1215}
};

const double LDR_POLY_COEFF[4][2] =
{
  {574.4643065, 0.256102471},
  {276.956799,	0.354229824},
  {456.8714075, 0.289840675},
  {1188.998156, 0.164731073}
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
SemaphoreHandle_t mutexArea;

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

  // Configura o servidor MQTT e o cliente MQTT
  mqttClient.setServer(MQTT_BROKER_IP_ADDRESS, 1883);
  mqttClient.setCallback(mqttCallback);
  reconnect();

  char strValue[20];
  sprintf(strValue, "%u", lightMode);

  mqttClient.publish("esp32/mode", strValue);

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
          Info_2 = tmp.a1;

          tmp = Area_1->getInformation();
          Info_1 = tmp.a0;
          Info_3 = tmp.a1;
          break;

        case 2:
          tmp = Area_0->getInformation();
          Info_0 = tmp.a0;

          tmp = Area_1->getInformation();
          Info_1 = tmp.a0;

          tmp = Area_2->getInformation();
          Info_2= tmp.a0;

          tmp = Area_3->getInformation();
          Info_3 = tmp.a0;

          break;

        case 3:
          tmp = Area_0->getInformation();
          Info_0 = tmp.a0;
          Info_1 = tmp.a1;

          tmp = Area_1->getInformation();
          Info_2 = tmp.a0;
          Info_3 = tmp.a1;

          break;

        case 4:
          tmp = Area_0->getInformation();
          Info_0 = tmp.a0;
          Info_1 = tmp.a1;
          Info_2 = tmp.a2;
          Info_3 = tmp.a3;
          break;
        
        case 5:
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
  if(readSensor){
    switch (lightMode)
    {
      case 0:
        Area_0->update(LUX_REFERENCE);
        Area_1->update(PWR_LED);
        Area_2->update(LUX_REFERENCE);          
        break;

      case 1:
        Area_0->update(LUX_REFERENCE);
        Area_1->update(PWR_LED);
        break;

      case 2:
        Area_0->update(LUX_REFERENCE);
        Area_1->update(LUX_REFERENCE);
        Area_2->update(LUX_REFERENCE);
        Area_3->update(LUX_REFERENCE);
        break;

      case 3:
        Area_0->update(LUX_REFERENCE);
        Area_1->update(LUX_REFERENCE);
        break;

      case 4:
        Area_0->update(LUX_REFERENCE);
        break;

      case 5:
        Area_0->update(PWR_LED);
        break;
      
      default:
        break;
    }
  }
  readSensor = false;
}

void areaInit(bool mode0, bool mode1, bool mode2, bool mode3){
  Serial.print("[I] Configurating areas");
  Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, AMOSTRAS_MED, LDR_POLY_COEFF[0], Lux_Reference[0], Kp, Ki, Kd, N, Ts, false);
  Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, AMOSTRAS_MED, LDR_POLY_COEFF[2], Lux_Reference[2], Kp, Ki, Kd, N, Ts, false);
  Area_1 = new Area(LDR1_PIN, LDR3_PIN, LED1_PIN, LED3_PIN, 1, 3, AMOSTRAS_MED, LDR_POLY_COEFF[1], LDR_POLY_COEFF[3], Lux_Reference[1], Lux_Reference[3], Kp, Ki, Kd, N, Ts, true);

  Serial.print(" [OK]\n");
}

void freeRTOSInit(){
  Serial.print("[I] Starting up FreeRTOS");
  mutexArea = xSemaphoreCreateMutex();
  
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
  while (readSensor == true)
  {
    delay(1);
  }

  timerAlarmDisable(readTimer);
  
  Serial.printf("Mudando para o modo %i\n", mode);
  switch(lightMode){
    case 0:
      delete Area_0, Area_1, Area_2;
      break;
    case 1:
      delete Area_0, Area_1;
      break;
    case 2:
      delete Area_0, Area_1, Area_2, Area_3;
      break;
    case 3:
      delete Area_0, Area_1;
      break;
    case 4:
      delete Area_0;
      break;
    case 5:
      delete Area_0;
      break;
    default:
      break;
  }    
  switch (mode)
  {
  case 0:
    lightMode = 0;
    Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, AMOSTRAS_MED, LDR_POLY_COEFF[0], Lux_Reference[0], Kp, Ki, Kd, N, Ts, false);
    Area_1 = new Area(LDR1_PIN, LDR3_PIN, LED1_PIN, LED3_PIN, 1, 3, AMOSTRAS_MED, LDR_POLY_COEFF[1], LDR_POLY_COEFF[3], Lux_Reference[1], Lux_Reference[3], Kp, Ki, Kd, N, Ts, true);
    Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, AMOSTRAS_MED, LDR_POLY_COEFF[2], Lux_Reference[2], Kp, Ki, Kd, N, Ts, false);
    mqttClient.publish("esp32/mode", "0");
    break;

  case 1:
    lightMode = 1;
    Area_0 = new Area(LDR0_PIN, LDR2_PIN, LED0_PIN, LED2_PIN, 0, 2, AMOSTRAS_MED, LDR_POLY_COEFF[0], LDR_POLY_COEFF[2], Lux_Reference[0], Lux_Reference[2], Kp, Ki, Kd, N, Ts, false);
    Area_1 = new Area(LDR1_PIN, LDR3_PIN, LED1_PIN, LED3_PIN, 1, 3, AMOSTRAS_MED, LDR_POLY_COEFF[1], LDR_POLY_COEFF[3], Lux_Reference[1], Lux_Reference[3], Kp, Ki, Kd, N, Ts, true);
    mqttClient.publish("esp32/mode", "1");
    break;

  case 2:
    lightMode = 2;
    Area_0 = new Area(LDR0_PIN, LED0_PIN, 0, AMOSTRAS_MED, LDR_POLY_COEFF[0], Lux_Reference[0], Kp, Ki, Kd, N, Ts, false);
    Area_1 = new Area(LDR1_PIN, LED1_PIN, 1, AMOSTRAS_MED, LDR_POLY_COEFF[1], Lux_Reference[1], Kp, Ki, Kd, N, Ts, false);
    Area_2 = new Area(LDR2_PIN, LED2_PIN, 2, AMOSTRAS_MED, LDR_POLY_COEFF[2], Lux_Reference[2], Kp, Ki, Kd, N, Ts, false);
    Area_3 = new Area(LDR3_PIN, LED3_PIN, 3, AMOSTRAS_MED, LDR_POLY_COEFF[3], Lux_Reference[3], Kp, Ki, Kd, N, Ts, false);
    mqttClient.publish("esp32/mode", "2");
    break;

  case 3:
    lightMode = 3;
    Area_0 = new Area(LDR0_PIN, LDR1_PIN, LED0_PIN, LED1_PIN, 0, 1, AMOSTRAS_MED, LDR_POLY_COEFF[0], LDR_POLY_COEFF[1], Lux_Reference[0], Lux_Reference[1], Kp, Ki, Kd, N, Ts, false);
    Area_1 = new Area(LDR2_PIN, LDR3_PIN, LED2_PIN, LED3_PIN, 2, 3, AMOSTRAS_MED, LDR_POLY_COEFF[2], LDR_POLY_COEFF[3], Lux_Reference[0], Lux_Reference[1], Kp, Ki, Kd, N, Ts, false);
    mqttClient.publish("esp32/mode", "3");
    break;

  case 4:
    lightMode = 4;
    Area_0 = new Area(LDR0_PIN, LDR1_PIN, LDR2_PIN, LDR3_PIN, LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN, 0, 1, 2, 3, AMOSTRAS_MED, LDR_POLY_COEFF[0], LDR_POLY_COEFF[1], LDR_POLY_COEFF[2], LDR_POLY_COEFF[3], Lux_Reference[0], Lux_Reference[1], Lux_Reference[2], Lux_Reference[3], Kp, Ki, Kd, N, Ts, false);
    mqttClient.publish("esp32/mode", "4");
    break;
  case 5:
    lightMode = 5;
    Area_0 = new Area(LDR0_PIN, LDR1_PIN, LDR2_PIN, LDR3_PIN, LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN, 0, 1, 2, 3, AMOSTRAS_MED, LDR_POLY_COEFF[0], LDR_POLY_COEFF[1], LDR_POLY_COEFF[2], LDR_POLY_COEFF[3], Lux_Reference[0], Lux_Reference[1], Lux_Reference[2], Lux_Reference[3], Kp, Ki, Kd, N, Ts, true);
    mqttClient.publish("esp32/mode", "5");
    break;
    
  default:
    break;
  }  

  timerAlarmWrite(readTimer, timerTime, true);
  timerAlarmEnable(readTimer);
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

  Serial.print("Mensagem MQTT recebida no topico [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

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
}


// const int numDataPoints = 101;
// int analogReadValues[numDataPoints] = {0, 699, 1107, 1335, 1494, 1645, 1751, 1836, 1915, 2011, 2072, 2137, 2186, 2247, 2280, 2327, 2371, 2401, 2431, 2475, 2511, 2524, 2576, 2586, 2619, 2638, 2668, 2685, 2719, 2716, 2769, 2792, 2798, 2814, 2813, 2849, 2850, 2865, 2878, 2894, 2907, 2911, 2935, 2954, 2966, 2979, 2967, 2982, 3019, 3007, 3017, 3055, 3047, 3054, 3062, 3072, 3080, 3087, 3138, 3105, 3117, 3121, 3154, 3140, 3165, 3169, 3186, 3190, 3173, 3184, 3185, 3243, 3204, 3208, 3226, 3232, 3262, 3239, 3261, 3255, 3268, 3260, 3273, 3307, 3293, 3304, 3294, 3308, 3325, 3321, 3316, 3317, 3325, 3332, 3351, 3349, 3350, 3358, 3345, 3346, 3375};
// float luxValues[numDataPoints] = {0, 2, 36, 51.2, 61.8, 77.1, 86, 100.5, 108.4, 124.9, 131.7, 148, 154, 166.1, 178.1, 188.4, 199, 211, 224, 233, 247, 256, 271, 278, 294, 302, 312, 324, 334, 347, 357, 371, 379, 395, 402, 418, 425, 442, 448, 466, 472, 481, 495, 504, 519, 528, 543, 551, 566, 573, 590, 596, 613, 619, 637, 643, 652, 666, 675, 690, 698, 714, 720, 737, 743, 760, 766, 785, 791, 800, 814, 824, 838, 846, 862, 869, 886, 892, 909, 915, 934, 940, 957, 963, 972, 987, 995, 1011, 1018, 1035, 1042, 1059, 1066, 1083, 1089, 1107, 1114, 1122, 1135, 1138, 1148};
// float convertToLux(int analogValue) {
//   // Find the two nearest analogRead values in the array
//   int index = 0;
//   while (index < numDataPoints - 1 && analogValue > analogReadValues[index + 1]) {
//     index++;
//   }

//   // Perform linear interpolation
//   float analogMin = analogReadValues[index];
//   float analogMax = analogReadValues[index + 1];
//   float luxMin = luxValues[index];
//   float luxMax = luxValues[index + 1];
//   float lux = ((analogValue - analogMin) * (luxMax - luxMin) / (analogMax - analogMin)) + luxMin;

//   return lux;
// }