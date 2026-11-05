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
#include <HTTPClient.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);
Botao btn();

const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* BROKER = "broker.emqx.io";

const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/comunicacao_entre_esp/pokedex";
const char* TOPICO_PUB = "senai134/esp32/comunicacao_entre_esp/treinador";

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
  Serial.printf("\n\rConectando ao Servidor do broker %s\n\r", BROKER);

  mqtt.setServer(BROKER, PORTA);
  //mqtt.setCallback();

  String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool brokerConectado = false;
  brokerConectado = mqtt.connect(id.c_str());

  if(brokerConectado){
    Serial.printf("\n\rBroker %s conectado com sucesso!\n\r", BROKER);
    mqtt.subscribe(TOPICO_SUB);
  } else{
    Serial.printf("Falha ao conectar ao broker %s\n\r", BROKER);
    Serial.print("Falha: ");
    Serial.println(mqtt.state());
  }
}

void loop() {
  mqtt.loop();

  
}

void aoReceber(char* topico, byte* payload, unsigned int tamanho){
  String mensagem;
  mensagem.reserve(tamanho);

  for(unsigned int i = 0; i < tamanho; i++){
    mensagem += (char)payload[i];
  }
  
  Serial.printf("Mensagem recebida no tópico: %s \n\r mensagem: %s\n\r", topico, mensagem.c_str());

  tratamento(mensagem);
}

void tratamento(String mensagem){
  JsonDocument doc;

  DeserializationError erro = deserializeJson(doc, mensagem);

  bool estadoLed = doc["estadoLed"].as<bool>();

  if(estadoLed){
    Serial.println("Led ligado");
  } else{
    Serial.println("Led desligado");
  }
}

/*
O esp B fica aguardando a mensagem do esp A, chegando os dados o esp B 
mostra os dados no display B, se for um pokémon raro (acho que tem esse 
bglh no Json da Pokedex) acende o led B. Apertando o botão B, mostra no
display B que capturou, e também envia essa mensagem para o esp A. 
Quando chega a mensagem no espelho A o led A acende e aparece no 
display A que foi capturado
*/