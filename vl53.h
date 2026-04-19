#pragma once
#include <Adafruit_VL53L0X.h>

void vl53Setup();
uint16_t lerDistancia();   // retorna distância em mm (8190 = fora de alcance)
bool obstaculoDetectado(); // true se distância < DIST_OBSTACULO