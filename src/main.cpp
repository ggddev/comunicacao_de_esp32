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
#include <Botao.h>

void aoReceber(char *, byte *, unsigned int);
void tratamento(String);
void exibirDadosDisplay(String);
void botaoEnviarMensagem();
void tempoLedLigado();

LiquidCrystal_I2C lcd(0x27, 20, 4);
Botao btnCaptura(0);
Botao btnNaoCapturou(15);

bool exibirNoLCD;

unsigned long ledDesligaEm = 0;

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";

const char *BROKER = "broker.emqx.io";

const int PORTA = 1883;
const char *TOPICO_SUB = "senai134/esp32/comunicacao_entre_esp/pokedex";   // eu recebo do esp do diogo
const char *TOPICO_PUB = "senai134/esp32/comunicacao_entre_esp/treinador"; // eu envio para o esp do diogo

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  btnCaptura.iniciar();
  btnNaoCapturou.iniciar();
  lcd.init();
  lcd.backlight();

  WiFi.begin(SSID, SENHA);
  while (WiFi.status() != WL_CONNECTED)
  {
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

  if (brokerConectado)
  {
    Serial.printf("\n\rBroker %s conectado com sucesso!\n\r", BROKER);
    mqtt.subscribe(TOPICO_SUB);
  }
  else
  {
    Serial.printf("Falha ao conectar ao broker %s\n\r", BROKER);
    Serial.print("Falha: ");
    Serial.println(mqtt.state());
  }
}

void loop()
{
  btnCaptura.atualizar();
  btnNaoCapturou.atualizar();
  mqtt.loop();

  if (btnCaptura.pressionou())
  {
    botaoEnviarMensagem();
  }

  if (btnNaoCapturou.pressionou())
  {
    botaoEnviarMensagem();
  }

  tempoLedLigado();
  exibirDadosDisplay("");
}

void aoReceber(char *topico, byte *payload, unsigned int tamanho)
{
  String mensagem;
  mensagem.reserve(tamanho);

  for (unsigned int i = 0; i < tamanho; i++)
  {
    mensagem += (char)payload[i];
  }

  if (mensagem == "")
  {
    exibirNoLCD = false;
  }

  exibirDadosDisplay(mensagem);
  
  Serial.printf("Mensagem recebida no tópico: %s \n\r mensagem: %s\n\r", topico, mensagem.c_str());

  tratamento(mensagem);
}

void tempoLedLigado()
{
  int temporizador = millis();
  int tempoLimite = 3000 + millis();
  if (tempoLimite > temporizador)
  {
    rgbLedWrite(48, 0, 0, 0);
  }
}

void tratamento(String mensagem)
{
  JsonDocument doc;

  DeserializationError erro = deserializeJson(doc, mensagem);

  bool mensagemRecebida = doc["msg"].as<const char *>();
  bool pokemonIdRecebido = doc["id"].as<int>();
  bool pokemonNomeRecebido = doc["nome"].as<const char *>();
  bool pokemonAlturaRecebido = doc["altura"].as<const char *>();
  bool pokemonPesoRecebido = doc["peso"].as<const char *>();

  if (pokemonIdRecebido)
  {
    Serial.println("Led ligado");
    rgbLedWrite(48, 0, 0, 255);
    delay(3000);
  }
  else
  {
    Serial.println("Led desligado");
    rgbLedWrite(48, 0, 0, 0);
  }
}

void exibirDadosDisplay(String mensagem)
{
  if(mensagem == "") return;
  lcd.clear();

  JsonDocument doc;

  DeserializationError deserializeError = deserializeJson(doc, mensagem);

  String nome = "";
  if (doc["nome"].is<const char *>())
    nome = "Nome: " + doc["nome"].as<String>();

  String id = "";
  if (doc["id"].is<int>())
    id = "ID: " + String(doc["id"].as<int>());

  String altura = "";
  if (doc["altura"].is<int>())
    altura = "Altura: " + String(doc["altura"].as<int>());

  String peso = "";
  if (doc["peso"].is<int>())
    peso = "Peso: " + String(doc["peso"].as<int>());

  if (!deserializeError)//esta deserializeError vai fazer a mesma função da variavel exibirNoLcd (neste cenario)
  {
    lcd.setCursor(0, 0);
    lcd.print(id);
    lcd.setCursor(0, 1);
    lcd.print(nome);
    lcd.setCursor(0, 2);
    lcd.print(altura);
    lcd.setCursor(0, 3);
    lcd.print(peso);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Erro no Json");
    lcd.setCursor(0,1);
    lcd.print(deserializeError.c_str());
  }
}

void botaoEnviarMensagem()
{
  static unsigned long ultimoEnvio = 0;
  const unsigned long tempoEntreEnvios = 5000;

  bool capturado;

  if (btnCaptura.clicou())
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Pokemon Capturado!");
    capturado = true;
  }
  if (btnNaoCapturou.clicou())
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Pokemon Nao Capturado");
    capturado = false;
  }

  if (millis() - ultimoEnvio > tempoEntreEnvios)
  {
    ultimoEnvio = millis();

    JsonDocument doc;
    doc["capturado"] = capturado;

    String mensagem;
    serializeJson(doc, mensagem);

    mqtt.publish(TOPICO_PUB, mensagem.c_str());
    Serial.printf("Mensagem a ser enviada no tópico %s: \n\r%s\n\r", TOPICO_PUB, mensagem.c_str());
  }
}