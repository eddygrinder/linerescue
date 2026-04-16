// rgb.h
#pragma once
#include <stdint.h>
#include <Wire.h>

#define VEML6040_ADDR 0x10

#define COR_BRANCO 0
#define COR_VERDE  1
#define COR_PRETO  2

struct RGBW {
    uint16_t r, g, b, w;
};

// setup
void rgbSetup();

// chamar no topo do loop — lê sensores e atualiza tudo
void rgbUpdate();

// estado atual dos sensores
bool esqBranco();
bool dtoBranco();
bool esqPreto();
bool dtoPreto();

// deteção de verde
bool verdeDecisaoCompleta();
bool verdeDuploDetectado();
bool verdeESQDetectado();
bool verdeDTODetectado();

// controlo
void resetarVerde();
void ignorarVerdePor(unsigned long ms);