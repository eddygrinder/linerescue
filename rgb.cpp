#include "rgb.h"
#include <Wire.h>
#include "config.h"

// ── janela temporal ───────────────────────────────────────

static bool vistoPorEsq = false;
static bool vistoPorDto = false;
static bool janelaAtiva = false;
static unsigned long tsInicio = 0;
static unsigned long ignorarVerdeAte = 0;

#define JANELA_VERDE_MS 600 // tempo para o 2º sensor aparecer ou para decidir que não é verde

// ── configuração ──────────────────────────────────────────
void configurarSensor(TwoWire &bus)
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
uint16_t readVEML(TwoWire &bus, uint8_t reg)
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

RGBW lerSensor(TwoWire &bus)
{
    RGBW val;
    val.r = readVEML(bus, 0x08);
    val.g = readVEML(bus, 0x09);
    val.b = readVEML(bus, 0x0A);
    val.w = readVEML(bus, 0x0B);
    return val;
}

// ── deteção verde ─────────────────────────────────────────
bool isGreen(uint16_t r, uint16_t g, uint16_t b, uint16_t w)
{
    if (w > LIMIAR_BRANCO_MAX)
        return false;
    if (w < LIMIAR_PRETO_MIN)
        return false;
    uint16_t total = r + g + b;
    if (total == 0)
        return false;
    return ((float)g / total) > LIMIAR_RATIO_VERDE;
}

void atualizarVerde()
{
    RGBW esq = lerSensor(Wire);
    RGBW dto = lerSensor(Wire1);

    Serial.print("W_ESQ=");
    Serial.print(esq.w);
    Serial.print(" W_DTO=");
    Serial.print(dto.w);
    Serial.print(" isG_ESQ=");
    Serial.print(isGreen(esq.r, esq.g, esq.b, esq.w));
    Serial.print(" isG_DTO=");
    Serial.println(isGreen(dto.r, dto.g, dto.b, dto.w));
    if (millis() < ignorarVerdeAte)
    {
        resetarVerde();
        return;
    }

    unsigned long agora = millis();
    //RGBW esq = lerSensor(Wire);
    //RGBW dto = lerSensor(Wire1);

    if (isGreen(esq.r, esq.g, esq.b, esq.w))
    {
        vistoPorEsq = true;
        if (!janelaAtiva)
        {
            janelaAtiva = true;
            tsInicio = agora;
        }
    }
    if (isGreen(dto.r, dto.g, dto.b, dto.w))
    {
        vistoPorDto = true;
        if (!janelaAtiva)
        {
            janelaAtiva = true;
            tsInicio = agora;
        }
    }
}

// Chamar no SEGUIR_LINHA para saber se já pode decidir
bool verdeDecisaoCompleta()
{
    if (!janelaAtiva)
        return false;
    // decide quando ambos viram OU quando a janela expirou
    return (vistoPorEsq && vistoPorDto) || (millis() - tsInicio > JANELA_VERDE_MS);
}

bool verdeDuploDetectado() { return vistoPorEsq && vistoPorDto; }
bool verdeESQDetectado() { return vistoPorEsq && !vistoPorDto; }
bool verdeDTODetectado() { return !vistoPorEsq && vistoPorDto; }
void resetarVerde()
{
    vistoPorEsq = false;
    vistoPorDto = false;
    janelaAtiva = false;
    tsInicio = 0;
}

void ignorarVerdePor(unsigned long ms)
{
    resetarVerde();
    ignorarVerdeAte = millis() + ms;
}