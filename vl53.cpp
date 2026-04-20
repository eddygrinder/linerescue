#include "vl53.h"
#include "config.h"

static VL53L0X sensor;

void vl53Setup()
{
    sensor.setTimeout(VL53_TIMEOUT_MS);
    if (!sensor.init())
    {
        Serial.println("VL53L0X não encontrado!");
        while (1)
            ;
    }
    // descomenta conforme necessário nos treinos:
    // sensor.setSignalRateLimit(0.1);  // LONG_RANGE
    // sensor.setMeasurementTimingBudget(20000);   // HIGH_SPEED
    // sensor.setMeasurementTimingBudget(200000);  // HIGH_ACCURACY
}

uint16_t lerDistancia()
{
    uint16_t d = sensor.readRangeSingleMillimeters();
    if (sensor.timeoutOccurred())
        return 8190;
    return d;
}

bool obstaculoDetectado()
{
    return lerDistancia() < DIST_OBSTACULO;
}