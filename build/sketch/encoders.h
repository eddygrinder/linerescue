#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\encoders.h"
#pragma once
#include <Arduino.h>
#include "config.h"    // pinos vêm daqui

#define ENC_FL  0
#define ENC_FR  1
#define ENC_TL  2
#define ENC_TR  3

void encoders_init();
void resetEncoders();
long getEncoder(uint8_t idx);
long ticksMedio();