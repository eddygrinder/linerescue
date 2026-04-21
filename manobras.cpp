#include "manobras.h"
#include "motores.h"
#include "encoders.h"
#include "config.h"
#include "sensores.h"
#include "vl53.h"

void virarEsquerda90()
{
    resetEncoders();
    setAllMotors(-VEL_VIRA, +VEL_VIRA, -VEL_VIRA, +VEL_VIRA);
    unsigned long inicio = millis();
    while (ticksMedio() < TICKS_90)
    {
        delay(10);
        if (millis() - inicio > TIMEOUT_MANOBRA_MS)
            break;
    }
    pararMotores();
    alinharNaLinha();
}

void virarDireita90()
{
    resetEncoders();
    setAllMotors(+VEL_VIRA, -VEL_VIRA, +VEL_VIRA, -VEL_VIRA);
    unsigned long inicio = millis();
    while (ticksMedio() < TICKS_90)
    {
        delay(10);
        if (millis() - inicio > TIMEOUT_MANOBRA_MS)
            break;
    }
    pararMotores();
    // alinharNaLinha();  ← desactivado — activar se necessário
}

void fazer180()
{
    resetEncoders();
    setAllMotors(-VEL_VIRA, +VEL_VIRA, -VEL_VIRA, +VEL_VIRA);
    unsigned long inicio = millis();
    while (ticksMedio() < TICKS_180)
    {
        delay(10);
        if (millis() - inicio > TIMEOUT_MANOBRA_MS)
            break;
    }
    pararMotores();
    delay(1000);
    alinharNaLinha();
}

void alinharNaLinha()
{
    qtr.readCalibrated(sensorValues);
    uint16_t pos = qtr.readLineBlack(sensorValues);
    int err = (int)pos - 3500;

    // desalinhamento pequeno → PID trata, não vale a pena alinhar
    if (abs(err) < LIMIAR_ALINHAMENTO)
        return;

    unsigned long tMax = millis() + TIMEOUT_ALINHAR_MS;
    while (millis() < tMax)
    {
        qtr.readCalibrated(sensorValues);
        pos = qtr.readLineBlack(sensorValues);
        err = (int)pos - 3500;

        if (abs(err) <= LIMIAR_ALINHAMENTO)
        {
            pararMotores();
            return;
        }

        if (err > 0)
            setAllMotors(-VEL_VIRA, +VEL_VIRA, -VEL_VIRA, +VEL_VIRA);
        else
            setAllMotors(+VEL_VIRA, -VEL_VIRA, +VEL_VIRA, -VEL_VIRA);
    }
    pararMotores();
}

void spinEsquerda(int vel)
{
    setAllMotors(-vel, vel, -vel, vel);
}

void lateralEsquerda(int vel)
{
    setAllMotors(vel, -vel, -vel, vel);
}

void lateraldireita(int vel)
{
    setAllMotors(-vel, vel, vel, -vel);
}

// manobras.cpp
void desviarEAvancar()
{
    // desliza esq enquanto vê obstáculo
    while (obstaculoDetectado())
    {
        moverLateral(ESQUERDA, VEL_DESVIO_LAT);
        delay(50);
    }
    pararMotores();
    delay(PAUSA_MOTORES_MS);

    // avança até encontrar linha
    setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
    while (!linhaDetectada())
    {
        qtr.readCalibrated(sensorValues);
    }
    pararMotores();
}