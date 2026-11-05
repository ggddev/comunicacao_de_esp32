/*
====================================
Programa: Comunicação de ESP32			      
Descrição: Foi preciso conectar os esps ao WiFi, ligar os LEDs, 
usar JSON para estruturação e troca de dados, usar Display LCD para
exibição de informações, como requisito opcional o consumo de API para 
requisição e tratamento dos dados, um botão para interação com usuário,
e utilizando MQTT para realizar a comunicação entre os dispositivos em 
formato JSON.
Autor: Guilherme Dantas           
Data: 04/11/2026                  
Versão 0.1
====================================
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Botao.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);
Botao btn();

const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* BROKER = "broker.emqx.io";

const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/comunicacao_entre_esp/comando";
const char* TOPICO_PUB = "senai134/esp32/comunicacao_entre_esp/status";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(SSID, SENHA);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  delay(1000);

  Serial.printf("\n\rConectado com sucesso a rede %s \n\r", SSID);
  Serial.printf("\n\rConectado ao Servidor do broker %s\n\r", BROKER);

  mqtt.setServer(BROKER, PORTA);
  //mqtt.setCallback();
}

void loop() {}