#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\vl53.cpp"
#include "vl53.h"
#include "config.h"

static Adafruit_VL53L0X lox;

void vl53Setup()
{
    if (!lox.begin())
    {
        Serial.println("VL53L0X não encontrado!");
        while (1)
            ;
    }
}

uint16_t lerDistancia()
{
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    if (measure.RangeStatus != 4)
        return measure.RangeMilliMeter;
    return 8190; // fora de alcance
}

bool obstaculoDetectado()
{
    return lerDistancia() < DIST_OBSTACULO;
}