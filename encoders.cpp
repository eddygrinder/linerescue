#include "encoders.h"

// ── Contadores (signed — detectam direcção) ──────────
volatile long encTicks[4] = {0, 0, 0, 0};

// ── Estado anterior de cada encoder ─────────────────
volatile uint8_t encEstado[4] = {0, 0, 0, 0};

// ── Tabela de quadratura ─────────────────────────────
// Índice = (estado_anterior << 2) | estado_actual
// +1 = CW,  -1 = CCW,  0 = inválido
static const int8_t QUAD_TABLE[16] = {
    0, +1, -1, 0,
    -1, 0, 0, +1,
    +1, 0, 0, -1,
    0, -1, +1, 0};

// ── ISR genérica ─────────────────────────────────────
static void processaEncoder(uint8_t idx, uint8_t pinA, uint8_t pinB)
{
    uint8_t estado_actual = (digitalRead(pinA) << 1) | digitalRead(pinB);
    uint8_t chave = (encEstado[idx] << 2) | estado_actual;
    encTicks[idx] += QUAD_TABLE[chave];
    encEstado[idx] = estado_actual;
}
 
// ── ISR individuais ──────────────────────────────────
void isrFL() { processaEncoder(ENC_FL, PIN_C1_FL, PIN_C2_FL); }
void isrFR() { processaEncoder(ENC_FR, PIN_C1_FR, PIN_C2_FR); }
void isrTL() { processaEncoder(ENC_TL, PIN_C1_TL, PIN_C2_TL); }
void isrTR() { processaEncoder(ENC_TR, PIN_C1_TR, PIN_C2_TR); }

// ── API pública ──────────────────────────────────────
void encoders_init()
{
    // Configurar pinos
    pinMode(PIN_C1_FL, INPUT_PULLUP);
    pinMode(PIN_C2_FL, INPUT_PULLUP);
    pinMode(PIN_C1_FR, INPUT_PULLUP);
    pinMode(PIN_C2_FR, INPUT_PULLUP);
    pinMode(PIN_C1_TL, INPUT_PULLUP);
    pinMode(PIN_C2_TL, INPUT_PULLUP);
    pinMode(PIN_C1_TR, INPUT_PULLUP);
    pinMode(PIN_C2_TR, INPUT_PULLUP);

    // Estado inicial
    encEstado[ENC_FL] = (digitalRead(PIN_C1_FL) << 1) | digitalRead(PIN_C2_FL);
    encEstado[ENC_FR] = (digitalRead(PIN_C1_FR) << 1) | digitalRead(PIN_C2_FR);
    encEstado[ENC_TL] = (digitalRead(PIN_C1_TL) << 1) | digitalRead(PIN_C2_TL);
    encEstado[ENC_TR] = (digitalRead(PIN_C1_TR) << 1) | digitalRead(PIN_C2_TR);

    // Interrupções em ambos os canais (quadratura completa)
    attachInterrupt(digitalPinToInterrupt(PIN_C1_FL), isrFL, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_C2_FL), isrFL, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_C1_FR), isrFR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_C2_FR), isrFR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_C1_TL), isrTL, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_C2_TL), isrTL, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_C1_TR), isrTR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_C2_TR), isrTR, CHANGE);
}

void resetEncoders()
{
    noInterrupts();
    encTicks[0] = encTicks[1] = encTicks[2] = encTicks[3] = 0;
    interrupts();
}

long getEncoder(uint8_t idx)
{
    noInterrupts();
    long val = encTicks[idx];
    interrupts();
    return val;
}

long ticksMedio()
{
    noInterrupts();
    long t = (abs(encTicks[0]) + abs(encTicks[1]) +
              abs(encTicks[2]) + abs(encTicks[3])) /
             4;
    interrupts();
    return t;
}