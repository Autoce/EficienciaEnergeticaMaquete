#define LDR0_PIN 33
#define LDR1_PIN 32
#define LDR2_PIN 35
#define LDR3_PIN 34

#define LED0_PIN 19
#define LED1_PIN 21
#define LED2_PIN 22
#define LED3_PIN 23

#define INIT_REFERENCE 300
#define INIT_POWER 0
#define INIT_KP 1
#define INIT_KI 5
#define INIT_KD 0
#define INIT_SAMPLING_TIME 0.001

#define AMOSTRAS_MED 100

#define UPDATE_TIME 5//Mili seconds

#define BLYNK_TEMPLATE_ID "TMPL3wq7SNoT"
#define BLYNK_TEMPLATE_NAME "Prototipo"
#define BLYNK_AUTH_TOKEN "OQCjaJYPjTqnG9501od-WtMmkEQzHpMD"
//#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "LARM_ALUNOS";
char pass[] = "LarmUfscq2022";

//Defina MQTT credentials
#define MQTT_BROKER_IP_ADDRESS "150.162.234.69"
#define MQTTUSERNAME "homeassistant"
#define MQTTPWD "ieZo8aepiu3eev6cheifeechee7weichoez9mai1Kee6ech2PaeYahcheth8sar7"