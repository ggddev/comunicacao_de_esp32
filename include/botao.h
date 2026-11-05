#ifndef BOTAO_H
#define BOTAO_H
#include <Arduino.h>

class Botao{

private:
    uint8_t _pinoBotao;
    bool _estadoBotaoAtual;
    bool _estadoBotaoAnterior = LOW;
    bool _pressionou = false;
    bool _soltou = false;
    uint32_t _ultimaMudancaMs = 0;
    uint32_t _tempoDebounceMs = 20;
    bool _estadoUltimaAcao = HIGH;
    
    uint32_t tempoDecorrido();

public: 
    Botao(uint8_t pinoBotao);

    void iniciar();
    void atualizar();
    bool pressionou();
    bool soltou();
};

#endif