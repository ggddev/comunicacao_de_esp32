#include "led.h"

Led::Led(uint8_t pino) : _pinoLed(pino){}

void Led::ligar(){
    _estadoLed = HIGH;
}

void Led::desligar(){
    _estadoLed = LOW;
}

void Led::ativarPiscar(uint32_t tempoEspera){
    _estaPiscando = true;
    _tempoEsperaAlternarMs = tempoEspera;
} 

void Led::iniciar(){
    pinMode(_pinoLed, OUTPUT);
    digitalWrite(_pinoLed, _estadoLed);
    _tempoAcaoAnteriorMs = millis();
}

void Led::atualizar(){
    if(_estaPiscando){
        const uint32_t tempoDecorrido = millis() - _tempoAcaoAnteriorMs; 
        
        if(tempoDecorrido >= _tempoEsperaAlternarMs){
            _tempoAcaoAnteriorMs = millis();
            alternar();
        }
    }

    digitalWrite(_pinoLed, _estadoLed);
}

void Led::desativarPiscar(){
    _estaPiscando = false;
    _estadoLed = LOW;
}

void Led::alternar(){
    _estadoLed = !_estadoLed;
}

uint8_t Led::getPinoLed(){
    return _pinoLed;
}

void Led::setEstadoLedf(bool estado){
    _estadoLed = estado;
}