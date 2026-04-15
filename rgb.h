#pragma once
#include "stdint.h"
void rgbSetup();

struct RGBW {
    uint16_t r, g, b, w;
};

#define VEML6040_ADDR 0x10 

bool verdeESQDetectado();
bool verdeDTODetectado();
void ignorarVerdePor(unsigned long ms);
void atualizarVerde();
bool verdeDuploDetectado();
void resetarVerde();
bool verdeDecisaoCompleta();
bool esqPreto();
bool dtoPreto();