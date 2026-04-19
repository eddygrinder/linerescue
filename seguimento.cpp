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

void seguirLinhaSubida(int erro) {
    float correcao = KP_SUBIR * erro + KD_SUBIR * (erro - erro_anterior);
    erro_anterior = erro;
    
    int esq = constrain(VEL_SUBIR - (int)correcao, 5, 80);
    int dir = constrain(VEL_SUBIR + (int)correcao, 5, 80);
    
    // frente mais rápida na subida
    setAllMotors(esq + 10, dir + 10, esq, dir);
}

void seguirLinhaDescida(int erro) {
    float correcao = KP_DESCER * erro + KD_DESCER * (erro - erro_anterior);
    erro_anterior = erro;
    
    int esq = constrain(VEL_DESCER - (int)correcao, 5, 40);
    int dir = constrain(VEL_DESCER + (int)correcao, 5, 40);
    
    // trás mais rápida na descida
    setAllMotors(esq, dir, esq + 5, dir + 5);
}