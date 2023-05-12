//Bibliotecas utilizadas
#include <ESP8266WiFi.h>
#include <FS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
 
//Definição dos pinos
#define Led 2

String webPage = "";
String stylePage = "";
String scriptPage = "";

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", webPage);
}

void handle_led_on(){
  digitalWrite(Led, LOW);
}

void handle_led_off(){
  digitalWrite(Led, HIGH);
}

void handleRootS(){
  server.send(200, "text/css", stylePage);
}

void handleRootJS(){
  server.send(200, "text/javascript", scriptPage);
}
 
void readFile(void) {
  //Faz a leitura do arquivo HTML
  File rFile = SPIFFS.open("/index.html","r");
  Serial.println("Lendo arquivo HTML...");
  webPage = rFile.readString();
  Serial.println(webPage);
  rFile.close();  
}

void readFileS(void){
  File rFile = SPIFFS.open("/style.css","r");
  Serial.println("Lendo arquivo css...");
  stylePage = rFile.readString();
  Serial.println(stylePage);
  rFile.close();
}

void readFileJS(void){
  File rFile = SPIFFS.open("/script.js","r");
  Serial.println("Lendo arquivo script...");
  scriptPage = rFile.readString();
  Serial.println(scriptPage);
  rFile.close();
}

const char* WLAN_SSID = "#######";
const char* WLAN_PASS = "############" ;
/************************* Adafruit.io Configuração *********************************/
 
#define AIO_SERVER      "io.adafruit.com" //Adafruit Servidor
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "#############"      //Insira o usuario criado na adafruit io
#define AIO_KEY         "###############" //Insira a chave de comunicação obtida na adafruit io


 
//Váriáveis utilizadas
 
int conectado = 1;
int conexao = 0;
 
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

  SPIFFS.begin();

  if(SPIFFS.exists("/index.html"))
  {
    Serial.println("\n\nfile exists!");
  }
  else Serial.println("\n\nNo File :(");
 
  readFile();
  readFileS();
  readFileJS();
 
  //Definição de saida e entrada de cada pino
  pinMode(Led,OUTPUT);

  digitalWrite(Led, HIGH);

  //Declaração de Tópicos de Leitura
  mqtt.subscribe(&Light1);

  server.on("/", handleRoot);
  server.on("/style.css", handleRootS);
  server.on("/script.js", handleRootJS);
  server.on("/led5_on", handle_led_on);
  server.on("/led5_off", handle_led_off);

  server.begin();
 
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

  server.handleClient();
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

/*
#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define led 2

const char* ssid = "iagoduda";
const char* password = "12345678";

ESP8266WebServer server(80);
 
 
void setup() {
  Serial.begin(9600);
  

  pinMode(led, OUTPUT);


 
  if(SPIFFS.exists("/index.html"))
  {
    Serial.println("\n\nfile exists!");
  }
  else Serial.println("\n\nNo File :(");
 
  readFile();
  readFileS();
  readFileJS();
  
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  server.on("/", handleRoot);
  server.on("/style.css", handleRootS);
  server.on("/script.js", handleRootJS);
  server.on("/led5_on", handle_led_on);
  server.on("/led5_off", handle_led_off);

  server.begin();
}
 
void loop() {
    server.handleClient();
}
*/
