#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\config.h"
#pragma once

// ─── PID ────────────────────────────────────────────────────────
#define VEL_BASE     30
#define KP           0.065f
#define KD           0.0f

// ─── QTR ────────────────────────────────────────────────────────
#define NUM_SENSORS  8
#define LIMIAR_PRETO 800

// ─── PINOS MOTORES ───────────────────────────────────────────────
#define FL_PWM1 5
#define FL_PWM2 4
#define FL_EN   48
#define FL_ENB  46

#define FR_PWM1 6
#define FR_PWM2 7
#define FR_EN   50
#define FR_ENB  52

#define RL_PWM1 10
#define RL_PWM2 11
#define RL_EN   25
#define RL_ENB  23

#define RR_PWM1 2
#define RR_PWM2 3
#define RR_EN   44
#define RR_ENB  42

// ── Encoders ─────────────────────────────────────────
#define PIN_C1_FL  24
#define PIN_C2_FL  26
#define PIN_C1_FR  14
#define PIN_C2_FR  15
#define PIN_C1_TL   8
#define PIN_C2_TL   9
#define PIN_C1_TR  13
#define PIN_C2_TR  12

// ─── SISTEMA ─────────────────────────────────────────────────────
#define TICKS_REV        1440u  // 12 CPR × 30 (caixa) × 4 (quadratura)
#define DIAM_RODA        58.0f  // mm
#define DIST_EIXOS      110.0f  // mm (esquerda–direita)

// ─── MANOBRAS ────────────────────────────────────────────────────
#define TICKS_360  ((uint32_t)(DIST_EIXOS / DIAM_RODA * TICKS_REV))
#define TICKS_90   (TICKS_360 / 4)
#define TICKS_180  (TICKS_360 / 2)
#define VEL_VIRA   80u
#define TICKS_25MM  ((uint32_t)(25.0f / (PI * DIAM_RODA) * TICKS_REV))  // ≈ 198
#define TICKS_11MM  87 // 11mm é a distância do centro do robô à borda da roda, então 22mm é o diâmetro do círculo que a roda percorre ao pivotar

// Limiares VEML6040 — calibrados empiricamente
//                        Medido    Com margem
// Limiares VEML6040 — calibrados empiricamente
#define LIMIAR_BRANCO_MAX   2000   // W máximo para não ser branco
#define LIMIAR_PRETO_MAX     950   // W máximo para ser preto
#define LIMIAR_RATIO_VERDE  0.44f  // G/(R+G+B) mínimo para ser verde

