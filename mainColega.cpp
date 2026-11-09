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
Versão 1.0
====================================
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Botao.h>
#include <HTTPClient.h>



LiquidCrystal_I2C lcd(0x27, 20, 4);
Botao btn(0);




const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* enderecoAPI = "https://pokeapi.co/api/v2/pokemon/";

const char* BROKER = "broker.emqx.io";

const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/comunicacao_entre_esp/treinador"; // recebo do ESP B
const char* TOPICO_PUB = "senai134/esp32/comunicacao_entre_esp/pokedex";    // envio para o ESP B

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

unsigned long ledDesligaEm = 0; 

//*=========================PROTOTIPANDO FUNCOES====================================
String pedirParaApi(String);
String idAleatorio();

void aoReceber(char*, byte*, unsigned int);
void tratarJson(String);

void controlarLed();


//*====================SETUP==============================
void setup() {
  Serial.begin(115200);
  Serial.println();

  btn.iniciar();

  lcd.init();
  lcd.backlight();




  WiFi.begin(SSID, SENHA);
  lcd.setCursor(0,0);
  lcd.print("Conectando no WiFi..");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conectou com sucesso");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conectando ao ");
  lcd.setCursor(0,1);
  lcd.print("servidor broker");
  Serial.printf("\n\rConectado com sucesso a rede %s \n\r", SSID);
  Serial.printf("\n\rConectado ao Servidor do broker %s\n\r", BROKER);

  mqtt.setServer(BROKER, PORTA);
  mqtt.setCallback(aoReceber);
  String id = "esp32-D" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool brokerConectado = false;
  brokerConectado = mqtt.connect(id.c_str());

  if(brokerConectado){
    Serial.printf("\n\r Broker %s conectado com sucesso!\n\r", BROKER);
    mqtt.subscribe(TOPICO_SUB);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Conectou no serivdor");
    lcd.setCursor(0,1);
    lcd.print("broker");
    lcd.setCursor(0,2);
    lcd.print("Aperte o botao para");
    lcd.setCursor(0,3);
    lcd.print("Escolher um pokemon");
  } else{
    Serial.printf("Falha ao conectar ao broker %s\n\r", BROKER);
    Serial.print("Falha: ");
    Serial.println(mqtt.state());
  }

  

}

//*==================================LOOP=============================
void loop() 
{
  mqtt.loop();
  btn.atualizar();
  controlarLed();

  JsonDocument doc;
  String pokedex;

  if(btn.pressionou())
  {
  pokedex = pedirParaApi(idAleatorio()).c_str();

  DeserializationError erroJson = deserializeJson(doc, pokedex);

  Serial.printf("\n\rPokemon sorteado: ");


  String name = "";
    if(doc["name"].is<const char*>())
      name = "Nome: " + doc["name"].as<String>();

  String id = "";
    if (doc["id"].is<int>())
      id = "ID: " + String(doc["id"].as<int>());

  String height = "";
    if (doc["height"].is<int>())
      height = "Altura: " + String(doc["height"].as<int>());

  String weight = "";
    if (doc["weight"].is<int>())
      weight = "Peso: " + String(doc["weight"].as<int>());


    //*Imprimendo na Serial 
    Serial.println(name.c_str());
    Serial.println(id.c_str());
    Serial.println(height.c_str());
    Serial.println(weight.c_str());

    //*Imprimindo no LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(name);
    lcd.setCursor(0,1);
    lcd.print(id);
    lcd.setCursor(0,2);
    lcd.print(height);
    lcd.setCursor(0,3);
    lcd.print(weight);

    //* ======Enviando para o ESP-B============

    JsonDocument envio;
    envio["id"]     = doc["id"];
    envio["nome"]   = doc["name"];
    envio["altura"] = doc["height"];
    envio["peso"]   = doc["weight"];

    String mensagem;
    serializeJson(envio, mensagem);
    mqtt.publish(TOPICO_PUB, mensagem.c_str());
    Serial.printf("Enviado: %s\n\r", mensagem.c_str());
  


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
//*==========COMUNICACAO MQTT=======================

void aoReceber(char* topico, byte* payLoad, unsigned int tamanho)
  {
    String mensagem;
    mensagem.reserve(tamanho); //* Evita realocacoes enquanto a String cresce.
    
    for(unsigned int i = 0; i < tamanho; i++)
    {
      mensagem += (char)payLoad[i];
    }

    Serial.printf("Mensagem recebida no topico: %s \n\r mensagem: %s \n\r", topico, mensagem.c_str());

    tratarJson(mensagem);
  }

  void tratarJson(String mensagem)
{
  JsonDocument docB;

  DeserializationError codigoErro = deserializeJson(docB, mensagem);
  //verifica se recebe true ou false
  if (codigoErro || !docB["capturado"].is<bool>())
  {
    Serial.println("Mensagem invalida");
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("Funcao invalida");
    rgbLedWrite(48,255,0,0);          
    ledDesligaEm = millis() + 3000;
    return;
  }

  bool capturado = docB["capturado"].as<bool>();

  if (capturado)
  {
    Serial.println("O pokemon foi capturado");
    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print("O pokemon foi");
    lcd.setCursor(6,2);
    lcd.print("capturado");
    rgbLedWrite(48,0,255,0);        
  }
  else
  {
    Serial.println("Pokemon nao capturado");
    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print("O pokemon nao");
    lcd.setCursor(4,2);
    lcd.print("foi capturado");
    rgbLedWrite(48,0,0,255);         
  }

  ledDesligaEm = millis() + 3000;
}
  void controlarLed()
{
  if (ledDesligaEm != 0 && millis() >= ledDesligaEm)
  {
    rgbLedWrite(48, 0, 0, 0); 
    lcd.clear();
    ledDesligaEm = 0;
  }
}