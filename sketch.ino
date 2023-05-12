//Bibliotecas utilizadas
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
 
//Definição dos pinos
#define Led 2

const char* WLAN_SSID = "#############";
const char* WLAN_PASS = "#############" ;
/************************* Adafruit.io Configuração *********************************/
 
#define AIO_SERVER      "io.adafruit.com" //Adafruit Servidor
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "############"      //Insira o usuario criado na adafruit io
#define AIO_KEY         "#############" //Insira a chave de comunicação obtida na adafruit io
 
//Váriáveis utilizadas
 
int conectado = 1;
int conexao = 0;
 
unsigned long anterior = 0;
unsigned long intervalo = 15000;
unsigned long atual;
 
//Definição do Wifi Client
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
 
//Váriáveis criadas para comunicação com o MQTT
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Led");        // a palavra feeds deve estar em todos
 
//Funções
void MQTT_connect(); //Determina a conexão MQTT
void Conectar();     //Verifica a conexão o Wifi
 
void setup() {
 
  Serial.begin(9600);
 
  //Definição de saida e entrada de cada pino
  pinMode(Led,OUTPUT);

  //Declaração de Tópicos de Leitura
  mqtt.subscribe(&Light1);

 
}
 
void loop() {
 
  Conectar();
  MQTT_connect();
 
  //Leitura do valor recebido através da Adafruit IO
  Adafruit_MQTT_Subscribe *subscription;
  if((subscription = mqtt.readSubscription(100))) {
    if (subscription == &Light1) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(Led, Light1_State);
    }
  }
}

 
void MQTT_connect() {
  int8_t ret;
  //Conectando MQTT
  if (mqtt.connected()) {
    if(conectado){
      conectado = 0;
      Serial.println("MQTT Conectado!");
    }
    return;
  }
  Serial.print("Conectando MQTT...");
  uint8_t retries = 3;
  if((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reconexao ao MQTT em 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    conectado = 1;
    //retries--;
    if (retries == 0) {
      //retries = 3;
    }
  }
}
 
void Conectar(){
  //Verifica a conexão wifi
  if(WiFi.status() != WL_CONNECTED){
      conexao = 0; 
      //Conectando ao Wifi
      Serial.println();
      Serial.println();
      Serial.print("Conectando a rede: ");
      Serial.println(WLAN_SSID);
      //Inicializa Conexão Wifi
      WiFi.begin(WLAN_SSID, WLAN_PASS);
      while(WiFi.status() != WL_CONNECTED) {
            delay(100);
            delay(100);
            Serial.print(".");
            conexao++;
            if(conexao == 30){
              ESP.restart();
            }
      }
      Serial.println("WiFi conectado!");
      Serial.println("Endereco de IP: ");
      Serial.println(WiFi.localIP());
      conectado = 1;
      Serial.println();
  }
}
