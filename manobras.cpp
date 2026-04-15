#include "manobras.h"
#include "motores.h"
#include "encoders.h"
#include "config.h"

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
}