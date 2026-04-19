#include "manobras.h"
#include "motores.h"
#include "encoders.h"
#include "config.h"
#include "sensores.h"

// FL_PWM1/2/EN/ENB, etc. vêm de config.h

void virarEsquerda90()
{
    resetEncoders();
    setAllMotors(-VEL_VIRA, +VEL_VIRA, -VEL_VIRA, +VEL_VIRA);

    unsigned long inicio = millis();
    while (ticksMedio() < TICKS_90)
    {
        delay(10);
        if (millis() - inicio > 2000)
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
        if (millis() - inicio > 2000)
            break;
    }
    pararMotores();
    //alinharNaLinha();
}

void fazer180()
{
    resetEncoders();
    // 360° = mesma direção de pivot, mas TICKS_360
    setAllMotors(-VEL_VIRA, +VEL_VIRA, -VEL_VIRA, +VEL_VIRA);

    unsigned long inicio = millis();
    while (ticksMedio() < TICKS_180)
    {
        delay(10);
        if (millis() - inicio > 2000)
            break;
    }
    pararMotores();
    delay(1000);
    alinharNaLinha();
}

void alinharNaLinha() {
    qtr.read(sensorValues);
    uint16_t pos = qtr.readLineBlack(sensorValues);
    int err = (int)pos - 3500;

    if (abs(err) < 1750) return;  // desalinhamento < 2 sensores → PID trata

    unsigned long tMax = millis() + 2000;
    while (millis() < tMax) {
        qtr.read(sensorValues);
        pos = qtr.readLineBlack(sensorValues);
        err = (int)pos - 3500;

        if (abs(err) <= 1750) { pararMotores(); return; }  // ← mesmo limiar → sai

        if (err > 0) setAllMotors(-VEL_VIRA, +VEL_VIRA, -VEL_VIRA, +VEL_VIRA);
        else         setAllMotors(+VEL_VIRA, -VEL_VIRA, +VEL_VIRA, -VEL_VIRA);
    }
    pararMotores();
}

void spinEsquerda(int vel) {
  // FL+RL avançam, FR+RR recuam
  setAllMotors(-vel, vel, -vel, vel);
}

void lateralEsquerda(int vel) {
  // FL+RL avançam, FR+RR recuam
  // FL, FR, RL, RR
  setAllMotors(vel, -vel, -vel, vel);
}

void lateraldireita(int vel) {
  // FL+RL avançam, FR+RR recuam
  // FL, FR, RL, RR
  setAllMotors(-vel, vel, vel, -vel);
}