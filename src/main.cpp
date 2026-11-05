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
#include <HTTPClient.h>
#include "led.h"
#include "botao.h"

void aoReceber(char*, byte*, unsigned int);
void tratamento(String);
void exibirDadosDisplay();

LiquidCrystal_I2C lcd(0x27, 20, 4);
Botao btnCaptura(0);
Led led(15);

String exibirNoLCD;


const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* BROKER = "broker.emqx.io";

const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/comunicacao_entre_esp/pokedex"; // eu recebo do esp do diogo
const char* TOPICO_PUB = "senai134/esp32/comunicacao_entre_esp/treinador"; //eu envio para o esp do diogo

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.println();
  btnCaptura.iniciar();
  led.iniciar();
  lcd.init();
  lcd.backlight();

  WiFi.begin(SSID, SENHA);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  delay(1000);

  Serial.printf("\n\rConectado com sucesso a rede %s \n\r", SSID);
  Serial.printf("\n\rConectando ao Servidor do broker %s\n\r", BROKER);

  mqtt.setServer(BROKER, PORTA);
  mqtt.setCallback(aoReceber);

  String id = "esp32G-" + String((uint32_t)ESP.getEfuseMac(), HEX);

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

  exibirDadosDisplay();
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

  exibirNoLCD = mensagem;
  
  Serial.printf("Mensagem recebida no tópico: %s \n\r mensagem: %s\n\r", topico, mensagem.c_str());

  tratamento(mensagem);
}

void tratamento(String mensagem){

  
  JsonDocument doc;

  DeserializationError erro = deserializeJson(doc, mensagem);

  bool estadoLed = doc["estadoLed"].as<bool>();

  if(estadoLed){
    Serial.println("Led ligado");
    rgbLedWrite(48, 0, 0, 255);
  } else{
    Serial.println("Led desligado");
    rgbLedWrite(48, 0,0,0);
  }
}

void exibirDadosDisplay(){
  if(exibirNoLCD != ""){
    lcd.setCursor(0,0);
    lcd.print(exibirNoLCD);
  } else{
    lcd.setCursor(0,0);
    lcd.print("Nao foi possivel exibir os dados.");
  }
}

/*
receber mensagens do mqtt vindas do esp-diogo

acender led ao receber mensagem

deserializar mensagem do diogo

botao funcionar

ao clicar no botao, mostrar no lcd a mensagem "Capturou!"

enviar essa mensagem para o esp-diogo

*/