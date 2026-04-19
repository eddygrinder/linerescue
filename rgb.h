#pragma once
#include <stdint.h>
#include <Wire.h>

#define VEML6040_ADDR 0x10

#define COR_BRANCO 0
#define COR_VERDE  1

void rgbSetup();
void rgbUpdate();

bool esqVerde();
bool dtoVerde();
bool esqVerdeValido();
bool dtoVerdeValido();

bool verdeDecisaoCompleta();
bool verdeDuploDetectado();
bool verdeESQDetectado();
bool verdeDTODetectado();

void resetarVerde();
void ignorarVerdePor(unsigned long ms);