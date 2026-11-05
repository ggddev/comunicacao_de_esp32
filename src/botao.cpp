#include "botao.h"

Botao::Botao(uint8_t pino) : _pinoBotao(pino){}

void Botao::iniciar(){
    pinMode(_pinoBotao, INPUT_PULLUP);
}

void Botao::atualizar(){
    _pressionou = false;
    _soltou = false;

    _estadoBotaoAtual = digitalRead(_pinoBotao);

    if(_estadoBotaoAtual != _estadoBotaoAnterior){

        _estadoBotaoAnterior = _estadoBotaoAtual;
        _ultimaMudancaMs = millis();
        return;

    }

    if(tempoDecorrido() < _tempoDebounceMs) return;

    if(_estadoBotaoAtual == _estadoBotaoAnterior) return;

    _estadoBotaoAnterior = _estadoBotaoAtual;

    const bool botaoPressionado = !_estadoBotaoAtual;

    botaoPressionado ? _pressionou = true : _soltou = true; 

}

bool Botao::pressionou(){
    return _pressionou;
}

bool Botao::soltou(){
    return _soltou;
}

uint32_t Botao::tempoDecorrido(){
    return millis()- _ultimaMudancaMs;
}