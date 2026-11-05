#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led
{

private:

  uint8_t _pinoLed;
  bool _estadoLed = 0;
  uint32_t _tempoAcaoAnteriorMs = 0;
  bool _estaPiscando = false;
  uint32_t _tempoEsperaAlternarMs = 0;

public:

  Led(uint8_t pinoLed);

  void ligar();
  void desligar();
  void ativarPiscar(uint32_t tempoEspera = 500);
  void iniciar();
  void atualizar();
  void alternar();
  void desativarPiscar();

  uint8_t getPinoLed();

  void setEstadoLedf(bool estado);
};

#endif