# ⚡🤖 Comunicação entre ESP32 utilizando MQTT e PokeAPI

## Descrição do Projeto

Este projeto foi desenvolvido com o objetivo de estabelecer uma comunicação entre dois microcontroladores ESP32, utilizando o protocolo MQTT para troca de informações.

A proposta consistiu em desenvolver um sistema de consulta e captura de Pokémon, utilizando a [PokeAPI](https://pokeapi.co/) como fonte de dados.

O projeto foi desenvolvido em dupla, com cada integrante responsável por um ESP32: **ESP-A** e **ESP-B**.

## Funcionamento

O sistema foi dividido em duas etapas principais:

### 1. Consulta e envio das informações (ESP-A)

O ESP-A é responsável por realizar as requisições à PokeAPI e obter as seguintes informações do Pokémon:

- ID;
- Nome;
- Altura;
- Peso.

Após obter e tratar os dados, o ESP-A aguarda o acionamento de um botão para enviar as informações ao ESP-B, utilizando o protocolo MQTT.

### 2. Recebimento e decisão de captura (ESP-B)

Ao receber uma nova mensagem, o ESP-B aciona seu LED RGB, indicando ao usuário que uma nova informação foi recebida. O LED permanece aceso por 3 segundos e, em seguida, é desligado.

As informações do Pokémon são apresentadas ao usuário por meio de um display. Com base nos dados exibidos, o usuário pode decidir se deseja capturar o Pokémon, utilizando dois botões:

- **Capturar:** caso o usuário escolha capturar o Pokémon, o sistema exibe a mensagem "Pokémon Capturado" no display do ESP-B e envia a confirmação ao ESP-A.
- **Não capturar:** caso o usuário opte por não capturar, o ESP-B exibe uma mensagem informando que o Pokémon não foi capturado e envia essa resposta ao ESP-A.

## Comunicação e sinalização (ESP-A)

Após receber a resposta do ESP-B, o ESP-A realiza a sinalização correspondente por meio de um LED e de um display:

| Resposta recebida | Sinalização no ESP-A |
|---|---|
| Pokémon capturado | LED verde e mensagem de confirmação no display. |
| Pokémon não capturado | LED azul e mensagem informando que a captura não foi realizada. |
| Mensagem inesperada ou inválida | LED vermelho e mensagem de erro no display. |

## Tratamento de Erros

Para garantir maior confiabilidade na comunicação, foi implementado um tratamento de erros no ESP-A.

Caso o ESP-A receba uma mensagem diferente das respostas esperadas, o sistema identifica a inconsistência, aciona o LED vermelho e apresenta uma mensagem de erro no display, informando que os dados recebidos não são válidos.

## Protocolo de Comunicação MQTT

A troca de informações entre os dois ESP32 é realizada por meio do protocolo MQTT, utilizando duas conexões independentes:

1. **Envio dos dados do Pokémon:** o ESP-A publica as informações obtidas na PokeAPI, enquanto o ESP-B recebe e processa os dados.
2. **Retorno da decisão de captura:** o ESP-B publica a resposta do usuário, enquanto o ESP-A recebe e interpreta a informação para executar a sinalização correspondente.

Essa divisão permite que os microcontroladores troquem informações nos dois sentidos, mantendo a comunicação organizada e independente para cada finalidade.

## Objetivos de Aprendizado

Com o desenvolvimento deste projeto, foram trabalhados os seguintes conceitos:

- Comunicação entre microcontroladores ESP32;
- Utilização do protocolo MQTT;
- Consumo de APIs REST utilizando a PokeAPI;
- Envio e recebimento de mensagens;
- Tratamento e validação de dados;
- Integração de botões, LEDs RGB e displays;
- Desenvolvimento de sistemas embarcados com comunicação bidirecional.

### Autores
Diogo Aguera Steinle,
Guilherme Gonçalves Dantas.
