#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\config.h"
#pragma once
#include <math.h>

// ─── PID — seguimento de linha ───────────────────────────────────
#define VEL_BASE 30 // velocidade base em linha recta
#define KP 0.03f    // ganho proporcional — afinar empiricamente
#define KD 0.0f     // ganho derivativo — afinar empiricamente

// ─── PID — subida de rampa ───────────────────────────────────────
#define VEL_SUBIR 80   // velocidade na subida
#define KP_SUBIR 0.02f // afinar empiricamente na rampa
#define KD_SUBIR 0.0f

// ─── PID — descida de rampa ──────────────────────────────────────
#define VEL_DESCER 15   // velocidade na descida — devagar para não embalar
#define KP_DESCER 0.05f // mais lento → mais correção necessária
#define KD_DESCER 0.0f

// ─── QTR ─────────────────────────────────────────────────────────
#define NUM_SENSORS 8
#define LIMIAR_PRETO 800         // valor calibrado (0-1000) acima do qual é preto
#define LIMIAR_PRETO_CENTRAL 600 // limiar mais permissivo para sensores centrais

// ─── PINOS MOTORES ───────────────────────────────────────────────
#define FL_PWM1 5
#define FL_PWM2 4
#define FL_EN 48
#define FL_ENB 46

#define FR_PWM1 6
#define FR_PWM2 7
#define FR_EN 50
#define FR_ENB 52

#define RL_PWM1 10
#define RL_PWM2 11
#define RL_EN 25
#define RL_ENB 23

#define RR_PWM1 2
#define RR_PWM2 3
#define RR_EN 44
#define RR_ENB 42

// ─── PINOS ENCODERS ──────────────────────────────────────────────
#define PIN_C1_FL 24
#define PIN_C2_FL 26
#define PIN_C1_FR 14
#define PIN_C2_FR 15
#define PIN_C1_TL 8
#define PIN_C2_TL 9
#define PIN_C1_TR 13
#define PIN_C2_TR 12

// ─── GEOMETRIA ───────────────────────────────────────────────────
#define TICKS_REV 1440u   // 12 CPR × 30 (caixa) × 4 (quadratura)
#define DIAM_RODA 58.0f   // mm
#define DIST_EIXOS 110.0f // mm (esquerda–direita)

// ─── MANOBRAS ────────────────────────────────────────────────────
#define TICKS_360 ((uint32_t)(DIST_EIXOS / DIAM_RODA * TICKS_REV))
#define TICKS_90 (TICKS_360 / 4)
#define TICKS_180 (TICKS_360 / 2)
#define VEL_VIRA 45u // velocidade de rotação nas viragens

// distâncias em ticks
#define TICKS_25MM ((uint32_t)(25.0f / (PI * DIAM_RODA) * TICKS_REV)) // ≈ 198
#define TICKS_CENTRO 210                                              // QTR sobre linha → centro do robot sobre interseção
#define TICKS_VERDE_PARA_LINHA 158                                    // QTR sobre verde → centro sobre linha preta

#define WAIT_VIRA_MS 500 // pausa antes de iniciar viragem (ms)

// ─── VEML6040 ────────────────────────────────────────────────────
#define LIMIAR_BRANCO_MAX 2000   // W acima disto → branco
#define LIMIAR_PRETO_MAX 900     // W abaixo disto → preto
#define LIMIAR_RATIO_VERDE 0.48f // G/(R+G+B) mínimo para verde
#define LIMIAR_PRATA_VEML 4000   // W acima disto → fita prateada (afinar no evento)
#define LIMIAR_PRATA 5 // valor bruto do QTR abaixo do qual pode ser prata (afinar no evento)

// ─── IMU LSM303 ──────────────────────────────────────────────────
#define OFFSET_X 0.04f    // aceleração X em repouso (calibrar no robot)
#define LIMIAR_RAMPA 1.5f // variação X acima disto → rampa (plano~0, rampa~3.35)

// ─── OBSTÁCULOS E RAMPAS ─────────────────────────────────────────
#define VEL_TRANSPOR 50 // velocidade para transpor debris/speedbump
// ─── VL53L0X ─────────────────────────────────────────────────────
#define DIST_OBSTACULO  200  // mm — distância de deteção do obstáculo (afinar nos treinos)

#define TICKS_DESVIO_LAT   700   // ~17.5cm lateral
#define TICKS_DESVIO_FREN  1350  // ~34cm frente
#define ESQUERDA 0
#define DIREITA  1