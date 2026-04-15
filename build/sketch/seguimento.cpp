#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\seguimento.cpp"
#include "seguimento.h"
#include "sensores.h"
#include "motores.h"
#include "config.h"
#include <Arduino.h>

static int erro_anterior = 0;

void seguimentoSetup() {
  erro_anterior = 0;
}

void seguirLinha(int erro) {
  static uint32_t t_anterior = 0;
  uint32_t agora = micros();
  uint32_t dt = agora - t_anterior;
  t_anterior = agora;

  float correcao = KP * erro + KD * (erro - erro_anterior);
  erro_anterior = erro;

  int esq = constrain(VEL_BASE - (int)correcao,5, 60);
  int dir = constrain(VEL_BASE + (int)correcao, 5, 60);

  setAllMotors(esq, dir, esq, dir);
}