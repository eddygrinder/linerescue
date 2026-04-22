#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\config.h"
#pragma once
#include <math.h>

// ╔══════════════════════════════════════════════════════════════════╗
// ║                    GUIA DE CALIBRAÇÃO                           ║
// ║                                                                  ║
// ║  Os valores marcados com ★ são os que precisam de ser           ║
// ║  ajustados conforme as condições do dia da prova.               ║
// ║  Os restantes não devem ser alterados sem orientação.           ║
// ╚══════════════════════════════════════════════════════════════════╝

// ─── PID — seguimento de linha ───────────────────────────────────
// ★ VEL_BASE: velocidade base do robot na linha recta
//   - Aumentar se o robot for lento a reagir às curvas
//   - Diminuir se o robot sair frequentemente da linha
#define VEL_BASE 30

// ★ KP: sensibilidade do PID à posição da linha (0.01 a 0.06)
//   - Aumentar se o robot reage devagar a desvios
//   - Diminuir se o robot oscila muito (zigzag)
#define KP 0.03f

// Não alterar sem testes cuidadosos
#define KD 0.0f

// ─── PID — rampa ─────────────────────────────────────────────────
// ★ VEL_SUBIR: velocidade na subida da rampa
//   - Aumentar se o robot para a meio da subida
//   - Diminuir se o robot sai da linha na subida
#define VEL_SUBIR 200
#define KP_SUBIR 0.03f
#define KD_SUBIR 0.0f

// ★ VEL_DESCER: velocidade na descida da rampa
//   - Deve ser baixa para o robot não embalar
//   - Aumentar ligeiramente se o robot para na descida
#define VEL_DESCER 15
#define KP_DESCER 0.05f
#define KD_DESCER 0.0f

// ─── QTR — sensor de linha ───────────────────────────────────────
#define NUM_SENSORS 8

// ★ LIMIAR_PRETO: valor acima do qual o sensor considera preto (0-1000)
//   - Calibrado automaticamente no arranque
//   - Só alterar se a calibração automática falhar
//   - Valores típicos: 700-900
#define LIMIAR_PRETO 800

// Não alterar — limiar interno para deteção de cruzamentos
#define LIMIAR_PRETO_CENTRAL 600

// ─── PINOS — não alterar ─────────────────────────────────────────
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

#define PIN_C1_FL 24
#define PIN_C2_FL 26
#define PIN_C1_FR 14
#define PIN_C2_FR 15
#define PIN_C1_TL 8
#define PIN_C2_TL 9
#define PIN_C1_TR 13
#define PIN_C2_TR 12

// ─── GEOMETRIA — não alterar ─────────────────────────────────────
// Características físicas do robot — só alterar se o hardware mudar
#define TICKS_REV 1440u   // encoders: 12 CPR × 30 (caixa) × 4 (quadratura)
#define DIAM_RODA 58.0f   // diâmetro das rodas em mm
#define DIST_EIXOS 110.0f // distância entre eixos esq-dto em mm

// ─── MANOBRAS ────────────────────────────────────────────────────
// Calculados automaticamente a partir da geometria — não alterar
#define TICKS_360 ((uint32_t)(DIST_EIXOS / DIAM_RODA * TICKS_REV))
#define TICKS_90  (TICKS_360 / 4)
#define TICKS_180 (TICKS_360 / 2)

// ★ VEL_VIRA: velocidade de rotação nas viragens a 90° e 180°
//   - Diminuir se o robot ultrapassa a linha após virar
//   - Aumentar se o robot não tem força para completar a viragem
#define VEL_VIRA 45u

// ★ TICKS_CENTRO: quantos ticks o robot avança após detetar linha lateral
//   antes de pivotar (centra o robot sobre a interseção)
//   - Aumentar se o robot pivota antes do centro estar sobre a linha
//   - Diminuir se o robot passa demasiado além da linha
#define TICKS_CENTRO 210

// ★ TICKS_VERDE_PARA_LINHA: ticks de avanço após detetar verde
//   antes de pivotar
#define TICKS_VERDE_PARA_LINHA 158

// ★ WAIT_VIRA_MS: pausa em ms antes de cada viragem
//   - Aumentar se os motores ainda estão em movimento ao iniciar a viragem
#define WAIT_VIRA_MS 500

// ─── VEML6040 — sensor de cor ────────────────────────────────────
// ★ Limiares de cor — ajustar se o robot não deteta verde corretamente
//   Usar o programa de diagnóstico RGB para medir os valores reais
//   Branco: W > 2000  |  Verde: 450 < G < 700  |  Preto: W < 900
#define LIMIAR_BRANCO_MAX 2000
#define LIMIAR_PRETO_MAX 900
#define LIMIAR_RATIO_VERDE 0.48f

// ★ LIMIAR_PRATA_VEML: valor W acima do qual é fita prateada
//   - Medir com o sensor sobre a fita no dia da prova e ajustar
#define LIMIAR_PRATA_VEML 4000
#define LIMIAR_PRATA 5

// ★ LIMIAR_RAMPA: inclinação mínima para considerar que está numa rampa
//   - Valor plano: ~0.04  |  Valor em rampa: ~3.35
//   - Ajustar se o robot não deteta a rampa ou deteta falsamente
#define LIMIAR_RAMPA 0.75f

// ─── OBSTÁCULOS ──────────────────────────────────────────────────
// ★ VEL_TRANSPOR_RAMPA: velocidade inicial para começar a subir a rampa
//   - Valor alto — precisa de força para vencer o desnível inicial
#define VEL_TRANSPOR_RAMPA 150

// ─── OBSTÁCULOS E RAMPAS ─────────────────────────────────────────
// ★ VEL_TRANSPOR_DEBRIS: velocidade para transpor debris (palitos, 3mm)
//   - Valor baixo — debris são pequenos
#define VEL_TRANSPOR_DEBRIS  50

#define VL53_TIMEOUT_MS  500  // timeout do sensor de distância (ms)

// ★ DIST_OBSTACULO: distância em mm a que o robot deteta o obstáculo
//   - Medir com o VL53L0X no dia da prova e ajustar
//   - Deve ser suficiente para o robot ter espaço para manobrar (~150-200mm)
#define DIST_OBSTACULO 200

// ─── DESVIO DE OBSTÁCULO ─────────────────────────────────────────
// ★ VEL_DESVIO_LAT: velocidade do movimento lateral para contornar obstáculo
//   - Diminuir se o robot desvia muito durante o deslize
#define VEL_DESVIO_LAT 75

#define TICKS_MEIO_ROBOT  325  // ~8.5cm (8cm + 5mm)
#define TICKS_EXTRA 30      // 5 mm - 20 ticks extra para garantir que o robot ultrapassa o obstáculo


// ★ TICKS_DESVIO_LAT: distância lateral de desvio em ticks (~18.5cm)
//   - Ajustar conforme a largura do obstáculo real na prova
#define TICKS_DESVIO_LAT 740

// ★ TICKS_DESVIO_FREN: distância de avanço para passar o obstáculo (~45cm)
//   - Ajustar conforme o comprimento do obstáculo real na prova
#define TICKS_DESVIO_FREN 1786

// Não alterar
#define ESQUERDA 0
#define DIREITA  1

// ★ PAUSA_MOTORES_MS: tempo de espera em ms entre mudanças de direcção
//   - Aumentar se os motores ainda estão em movimento ao mudar de direcção
#define PAUSA_MOTORES_MS 100

// ★ As velocidades mínimas e máximas para o controlo dos motores — ajustar se necessário
#define VEL_MIN          5   // velocidade mínima dos motores
#define VEL_MAX_LINHA   60   // velocidade máxima em linha recta
#define VEL_MAX_SUBIDA  80   // velocidade máxima na subida
#define VEL_MAX_DESCIDA 40   // velocidade máxima na descida
#define BOOST_FRENTE    10   // boost das rodas dianteiras na subida
#define BOOST_TRAS       5   // boost das rodas traseiras na descida

// manobras — Não deverá ser preciso alterar estes valores, mas podem ser ajustados se as manobras não estiverem a funcionar bem
#define TIMEOUT_MANOBRA_MS   2000  // timeout máximo para cada manobra (ms)
#define LIMIAR_ALINHAMENTO   1750  // erro QTR máximo antes de alinhar (~2 sensores)
#define TIMEOUT_ALINHAR_MS   2000  // timeout máximo para alinharNaLinha (ms)

// ─── RGB — sensor de cor VEML6040 ─────────────────────────────────
// Não deve ser preciso alterar
// Limiares de cor — ajustar se o robot não deteta verde corretamente
// Usar o programa de diagnóstico RGB para medir os valores reais no dia da prova
#define VERDE_G_MIN      450   // ★ valor mínimo do canal G para ser verde
#define VERDE_G_MAX      700   // ★ valor máximo do canal G para ser verde
#define JANELA_VERDE_MS  300   // ★ tempo máximo para o 2º sensor confirmar verde (ms)