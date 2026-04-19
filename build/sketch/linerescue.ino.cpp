#include <Arduino.h>
#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
#include "rgb.h"
#include "config.h"

// ── estado interno ────────────────────────────────────────
// cor actual de cada sensor (COR_VERDE ou COR_BRANCO)
static uint8_t _corEsq     = COR_BRANCO;
static uint8_t _corDto     = COR_BRANCO;

// cor do ciclo anterior — para validar verde (só válido se veio de branco)
static uint8_t anteriorEsq = COR_BRANCO;
static uint8_t anteriorDto = COR_BRANCO;

// flags de acumulação — ficam true assim que o sensor vê verde
static bool vistoPorEsq    = false;
static bool vistoPorDto    = false;

// janela temporal — activa quando o primeiro sensor vê verde
static bool janelaAtiva    = false;
static unsigned long tsInicio = 0;  // timestamp do início da janela

// período de imunidade após manobra — ignora verde durante X ms
static unsigned long ignorarVerdeAte = 0;

#define JANELA_VERDE_MS 300  // tempo máximo para o 2º sensor confirmar verde

// ── configuração ──────────────────────────────────────────
// configura o VEML6040 num barramento I2C
#line 28 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
static void configurarSensor(TwoWire &bus);
#line 36 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void rgbSetup();
#line 46 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
static uint16_t readVEML(TwoWire &bus, uint8_t reg);
#line 58 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void rgbUpdate();
#line 87 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool esqVerde();
#line 88 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool dtoVerde();
#line 91 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool esqVerdeValido();
#line 92 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool dtoVerdeValido();
#line 98 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool verdeDecisaoCompleta();
#line 103 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool verdeDuploDetectado();
#line 104 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool verdeESQDetectado();
#line 105 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool verdeDTODetectado();
#line 109 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void resetarVerde();
#line 116 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void ignorarVerdePor(unsigned long ms);
#line 125 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
bool fitaPrateadaDetectada();
#line 28 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
static void configurarSensor(TwoWire &bus) {
    bus.beginTransmission(VEML6040_ADDR);
    bus.write(0x00);  // registo CONF
    bus.write(0x00);  // IT 40ms, SD=0 (activo)
    bus.write(0x00);  // high byte
    bus.endTransmission(true);  // true = corrige bug repeated-start Due
}

void rgbSetup() {
    Wire.begin();  Wire.setClock(400000);   // sensor ESQ — 400kHz
    Wire1.begin(); Wire1.setClock(400000);  // sensor DTO — 400kHz
    configurarSensor(Wire);
    configurarSensor(Wire1);
    delay(100);  // aguarda primeira leitura válida
}

// ── leitura I2C ───────────────────────────────────────────
// lê um registo de 16 bits do VEML6040
static uint16_t readVEML(TwoWire &bus, uint8_t reg) {
    bus.requestFrom(VEML6040_ADDR, 2, (int)reg, 1, true);
    if (bus.available() == 2) {
        uint8_t low  = bus.read();
        uint8_t high = bus.read();
        return (high << 8) | low;
    }
    return 0;
}

// ── atualização principal ─────────────────────────────────
// chamar no topo do loop — lê canal G de cada sensor e actualiza estado
void rgbUpdate() {
    // só lê canal G (reg 0x09) — 2 leituras I2C em vez de 8
    // suficiente para distinguir verde (G~500-700) de branco (G>1000)
    uint16_t g_esq = readVEML(Wire,  0x09);
    uint16_t g_dto = readVEML(Wire1, 0x09);

    // guarda cor anterior antes de actualizar
    anteriorEsq = _corEsq;
    anteriorDto = _corDto;

    // classifica cor actual pelo valor absoluto de G
    _corEsq = (g_esq > 450 && g_esq < 700) ? COR_VERDE : COR_BRANCO;
    _corDto = (g_dto > 450 && g_dto < 700) ? COR_VERDE : COR_BRANCO;

    // durante período de imunidade — limpa flags e sai
    if (millis() < ignorarVerdeAte) { resetarVerde(); return; }

    // acumula flags de verde e inicia janela temporal
    if (_corEsq == COR_VERDE) {
        vistoPorEsq = true;
        if (!janelaAtiva) { janelaAtiva = true; tsInicio = millis(); }
    }
    if (_corDto == COR_VERDE) {
        vistoPorDto = true;
        if (!janelaAtiva) { janelaAtiva = true; tsInicio = millis(); }
    }
}

// ── estado actual dos sensores ────────────────────────────
bool esqVerde() { return _corEsq == COR_VERDE; }
bool dtoVerde() { return _corDto == COR_VERDE; }

// verde válido — só considera se veio de branco (não de preto/entroncamento)
bool esqVerdeValido() { return _corEsq == COR_VERDE && anteriorEsq == COR_BRANCO; }
bool dtoVerdeValido() { return _corDto == COR_VERDE && anteriorDto == COR_BRANCO; }

// ── decisão de verde ──────────────────────────────────────
// retorna true quando há informação suficiente para decidir:
// — ambos os sensores viram verde, OU
// — janela de 300ms expirou (só um sensor viu verde)
bool verdeDecisaoCompleta() {
    if (!janelaAtiva) return false;
    return (vistoPorEsq && vistoPorDto) || (millis() - tsInicio > JANELA_VERDE_MS);
}

bool verdeDuploDetectado() { return vistoPorEsq && vistoPorDto;  }  // dois verdes → inversão
bool verdeESQDetectado()   { return vistoPorEsq && !vistoPorDto; }  // só esq → vira esq
bool verdeDTODetectado()   { return !vistoPorEsq && vistoPorDto; }  // só dto → vira dto

// ── controlo ──────────────────────────────────────────────
// limpa todas as flags e histórico de cor
void resetarVerde() {
    vistoPorEsq = false; vistoPorDto = false;
    janelaAtiva = false; tsInicio    = 0;
    _corEsq = COR_BRANCO; _corDto = COR_BRANCO;  // reset histórico
}

// limpa flags e define período de imunidade após manobra
void ignorarVerdePor(unsigned long ms) {
    resetarVerde();
    ignorarVerdeAte = millis() + ms;
}

// ── fita prateada ─────────────────────────────────────────
// deteta a fita prateada da zona de evacuação pelo canal W (luminosidade)
// a prata reflecte muito mais luz que o branco normal (~3500)
// chamar pontualmente quando QTR sugere zona anormal (todos brancos)
bool fitaPrateadaDetectada() {
    uint16_t w_esq = readVEML(Wire,  0x0B);  // canal W sensor ESQ
    uint16_t w_dto = readVEML(Wire1, 0x0B);  // canal W sensor DTO
    return w_esq > LIMIAR_PRATA_VEML || w_dto > LIMIAR_PRATA_VEML;
}
