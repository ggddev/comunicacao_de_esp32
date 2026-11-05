/*
====================================
Programa: Comunicação de ESP32			      
Descrição: Foi preciso conectar os esps ao WiFi, ligar os LEDs, 
usar JSON para estruturação e troca de dados, usar Display LCD para
exibição de informações, como requisito opcional o consumo de API para 
requisição e tratamento dos dados, um botão para interação com usuário,
e utilizando MQTT para realizar a comunicação entre os dispositivos em 
formato JSON.
Autor: Diogo Aguera         
Data: 04/11/2026                  
Versão 0.1
====================================
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "led.h"
#include <Botao.h>
#include <HTTPClient.h>



LiquidCrystal_I2C lcd(0x27, 16, 2);
Botao btn(0);




const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* enderecoAPI = "https://pokeapi.co/api/v2/pokemon/";

const char* BROKER = "broker.emqx.io";

const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/comunicacao_entre_esp/pokedex";
const char* TOPICO_PUB = "senai134/esp32/comunicacao_entre_esp/treinador";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

//*=========================PROTOTIPANDO FUNCOES====================================
String pedirParaApi(String);
String idAleatorio();


//*====================SETUP==============================
void setup() {
  Serial.begin(115200);
  Serial.println();
  btn.iniciar();



  WiFi.begin(SSID, SENHA);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  delay(1000);

  Serial.printf("\n\rConectado com sucesso a rede %s \n\r", SSID);
  Serial.printf("\n\rConectado ao Servidor do broker %s\n\r", BROKER);

  mqtt.setServer(BROKER, PORTA);
  String id = "esp32-D" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool brokerConectado = false;
  brokerConectado = mqtt.connect(id.c_str());

  if(brokerConectado){
    Serial.printf("\n\r Broker %s conectado com sucesso!\n\r", BROKER);
    mqtt.subscribe(TOPICO_SUB);
  } else{
    Serial.printf("Falha ao conectar ao broker %s\n\r", BROKER);
    Serial.print("Falha: ");
    Serial.println(mqtt.state());
  }

  

}

//*==================================LOOP=============================
void loop() 
{
  btn.atualizar();

  JsonDocument doc;
  String pokedex;

  if(btn.pressionou())
  {
  pokedex = pedirParaApi(idAleatorio()).c_str();

  DeserializationError erroJson = deserializeJson(doc, pokedex);

  Serial.printf("\n\rPokemon sorteado: ");

  String name = "";
    if(doc["name"].is<const char*>())
    {
      name = "Nome: " + doc["name"].as<String>();
    }

    Serial.println(name.c_str());
  }
}

//* ==============FUNCOES===================
String pedirParaApi(String id)
{
  HTTPClient http;
  
  //id = idAleatorio();

  http.begin(enderecoAPI + id); //*pega o link do site e o final da URL para acessar a informacao desejada

  int codigoHTTP = http.GET(); //*pega o codigo da pagina (200) para verificar se a pagina existe (Retorna o codigo HTTP ex: 200 ok ou 404 nao encontrado)

  String resposta;

  if (codigoHTTP == 200)
  {
    resposta = http.getString(); //*Retorna o Conteudo HTML da pagina (pega a string)
  }
  else
  {
    Serial.printf("Deu erro. Codigo: %d", codigoHTTP);
  }
  http.end();
  return resposta;
}


String idAleatorio()
{
  String n = String(random(1, 152));

  return n;
}
