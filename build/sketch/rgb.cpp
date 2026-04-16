#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\rgb.cpp"
// rgb.cpp
#include "rgb.h"
#include "config.h"

// ── estado interno ────────────────────────────────────────
static uint8_t _corEsq = COR_BRANCO;
static uint8_t _corDto = COR_BRANCO;

static bool vistoPorEsq = false;
static bool vistoPorDto = false;
static bool janelaAtiva = false;
static unsigned long tsInicio = 0;
static unsigned long ignorarVerdeAte = 0;

#define JANELA_VERDE_MS 1000

// ── configuração ──────────────────────────────────────────
static void configurarSensor(TwoWire &bus)
{
    bus.beginTransmission(VEML6040_ADDR);
    bus.write(0x00);
    bus.write(0x00);
    bus.write(0x00);
    bus.endTransmission(true);
}

void rgbSetup()
{
    Wire.begin();
    Wire1.begin();
    configurarSensor(Wire);
    configurarSensor(Wire1);
    delay(100);
}

// ── leitura ───────────────────────────────────────────────
static uint16_t readVEML(TwoWire &bus, uint8_t reg)
{
    bus.requestFrom(VEML6040_ADDR, 2, (int)reg, 1, true);
    if (bus.available() == 2)
    {
        uint8_t low = bus.read();
        uint8_t high = bus.read();
        return (high << 8) | low;
    }
    return 0;
}

static RGBW lerSensor(TwoWire &bus)
{
    RGBW val;
    val.r = readVEML(bus, 0x08);
    val.g = readVEML(bus, 0x09);
    val.b = readVEML(bus, 0x0A);
    val.w = readVEML(bus, 0x0B);
    return val;
}

// ── interpretação ─────────────────────────────────────────
static bool isGreen(uint16_t r, uint16_t g, uint16_t b, uint16_t w)
{
    if (w > LIMIAR_BRANCO_MAX)
        return false;
    if (w < LIMIAR_PRETO_MAX)
        return false;
    uint16_t total = r + g + b;
    if (total == 0)
        return false;
    return ((float)g / total) > LIMIAR_RATIO_VERDE;
}

static bool isBlack(uint16_t w)
{
    return w < LIMIAR_PRETO_MAX;
}

// ── atualização principal — chamar no topo do loop ────────
void rgbUpdate()
{
    RGBW esq = lerSensor(Wire);
    RGBW dto = lerSensor(Wire1);

    // determina cor atual
    _corEsq = isGreen(esq.r, esq.g, esq.b, esq.w) ? COR_VERDE : isBlack(esq.w) ? COR_PRETO
                                                                               : COR_BRANCO;
    _corDto = isGreen(dto.r, dto.g, dto.b, dto.w) ? COR_VERDE : isBlack(dto.w) ? COR_PRETO
                                                                               : COR_BRANCO;

    // acumula flags de verde (só se não estiver em período de ignore)
    if (millis() < ignorarVerdeAte)
    {
        resetarVerde();
        return;
    }

    if (_corEsq == COR_VERDE)
    {
        vistoPorEsq = true;
        if (!janelaAtiva)
        {
            janelaAtiva = true;
            tsInicio = millis();
        }
    }
    if (_corDto == COR_VERDE)
    {
        vistoPorDto = true;
        if (!janelaAtiva)
        {
            janelaAtiva = true;
            tsInicio = millis();
        }
    }
}

// ── estado atual dos sensores ─────────────────────────────
bool esqBranco() { return _corEsq == COR_BRANCO; }
bool dtoBranco() { return _corDto == COR_BRANCO; }
bool esqPreto() { return _corEsq == COR_PRETO; }
bool dtoPreto() { return _corDto == COR_PRETO; }

// ── deteção de verde ──────────────────────────────────────
bool verdeDecisaoCompleta()
{
    if (!janelaAtiva)
        return false;
    return (vistoPorEsq && vistoPorDto) || (millis() - tsInicio > JANELA_VERDE_MS);
}

bool verdeDuploDetectado() { return vistoPorEsq && vistoPorDto; }
bool verdeESQDetectado() { return vistoPorEsq && !vistoPorDto; }
bool verdeDTODetectado() { return !vistoPorEsq && vistoPorDto; }

// ── controlo ──────────────────────────────────────────────
void resetarVerde()
{
    vistoPorEsq = false;
    vistoPorDto = false;
    janelaAtiva = false;
    tsInicio = 0;
    _corEsq = COR_BRANCO; // ← reset do histórico
    _corDto = COR_BRANCO; // ← reset do histórico
}

void ignorarVerdePor(unsigned long ms)
{
    resetarVerde();
    ignorarVerdeAte = millis() + ms;
}
