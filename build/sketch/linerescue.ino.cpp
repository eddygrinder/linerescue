#include <Arduino.h>
#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
#include "motores.h"
#include "sensores.h"
#include "seguimento.h"
#include "manobras.h"
#include "encoders.h"
#include "rgb.h"

// ─── ESTADOS ─────────────────────────────────────────────────────
enum Estado
{
  CALIBRAR,
  SEGUIR_LINHA,
  DESVIAR,      // reservado — VL53L0X
  SUBIR_RAMPA,  // reservado — acelerómetro
  DESCER_RAMPA, // reservado — acelerómetro
  ENTR_ESQ,     // reservado — sensores da esquerda veem linha e os da direita não
  ENTR_DTO,     // reservado — sensores da direita veem linha e os da esquerda não
  CRUZAMENTO,   // reservado — QTR padrão
  INVERSAO,     // reservado — ambos sensores veem verde,
  ATRAVESSAR,   // reservado — atravessa a linha às cegas até sair do preto
  PARAR
};

Estado estado = CALIBRAR;
bool verdeRecente = false;

// ─── SETUP ───────────────────────────────────────────────────────
#line 28 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void setup();
#line 68 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void loop();
#line 28 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void setup()
{
  Serial.begin(115200);
  motorSetup();
  encoders_init();
  sensoresSetup();
  rgbSetup();
  delay(500);

  calibrarQTR();
  seguimentoSetup();

  estado = SEGUIR_LINHA;
  Serial.println("A seguir linha...");
  delay(1000);
}

/*
loop()
  │
  ├── 1. lerSensores()          → array sensor[] preenchido
  ├── 2. lerUltrasonico()       → distancia_cm preenchido
  │
  ├── 3. Pré-calcular tudo
  │       posicao, erro
  │       lado_do_verde()
  │       cruzamento_detectado()
  │
  └── 4. switch(estado)
            SEGUIR_LINHA   → PID com erro já calculado
                           → testa verde com leitura já feita
                           → testa cruzamento com leitura já feita
                           → testa obstáculo com distancia já lida
            CRUZAMENTO     → decide virar/seguir/360
            SUBIR_RAMPA    → controlo de velocidade
            DESVIAR        → lógica de desvio
            PARAR          → para
*/

// ─── LOOP ────────────────────────────────────────────────────────
void loop()
{
  qtr.read(sensorValues);
  bool linha = linhaDetectada();
  rgbUpdate();

  uint16_t posicao = 3500;
  int erro = 0;
  if (linha)
  {
    posicao = qtr.readLineBlack(sensorValues);
    erro = (int)posicao - 3500;
  }
  switch (estado)
  {
  case SEGUIR_LINHA:
  /*  if (esqPreto() || dtoPreto())
    {
      estado = ATRAVESSAR;
    }
    else if (verdeDecisaoCompleta())
    {
      if (verdeDuploDetectado())
      {
        ignorarVerdePor(500);
        estado = INVERSAO;
      }
      else if (verdeESQDetectado())
      {
        ignorarVerdePor(100);
        estado = ENTR_ESQ;
      }
      else if (verdeDTODetectado())
      {
        ignorarVerdePor(100);
        estado = ENTR_DTO;
      }
    }*/
    seguirLinha(erro);
    break;
  case ATRAVESSAR:
    setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
    if (esqBranco() && dtoBranco())
    {
      resetarVerde(); // ← limpa qualquer verde acumulado durante o ATRAVESSAR
      estado = SEGUIR_LINHA;
    }
    break;
  case ENTR_ESQ:
    resetEncoders();
    setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
    while (ticksMedio() < 87)
    {
    }
    pararMotores();
    delay(200);
    virarEsquerda90();
    ignorarVerdePor(2000);
    estado = SEGUIR_LINHA;
    break;

  case ENTR_DTO:
    resetEncoders();
    setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
    while (ticksMedio() < 87)
    {
    }
    pararMotores();
    delay(200);
    virarDireita90();
    ignorarVerdePor(2000);
    estado = SEGUIR_LINHA;
    break;

  case INVERSAO:
    pararMotores();
    delay(200);
    fazer180();
    pararMotores();
    ignorarVerdePor(1000);
    estado = SEGUIR_LINHA;
    break;

  case PARAR:
    pararMotores();
    break;
  }
}
