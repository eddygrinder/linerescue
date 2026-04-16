#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\sensores.h"
#pragma once
#include <QTRSensors.h>
#include "config.h"

extern QTRSensors qtr;
extern uint16_t sensorValues[NUM_SENSORS];

void sensoresSetup();
void calibrarQTR();
uint16_t lerPosicaoLinha();    // devolve 0–7000
bool linhaDetectada();         // true se algum sensor vê linha
bool cruzamento_detectado();    // true se a maioria dos sensores vê linha
bool sensorEVerde(int idx);     // true se o sensor idx vê verde
bool verdeEsquerda();       // true se o sensor mais à esquerda vê verde
bool verdeDireita();        // true se o sensor mais à direita vê verde
bool entroncamentoEsq();       // true se os 4 sensores da esquerda vêem preto (entroncamento à esquerda)    