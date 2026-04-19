#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\imu.cpp"
#include "imu.h"
#include "config.h"

static Adafruit_LSM303_Accel_Unified accel(54321);
static float offsetX = 0;

void imuSetup() {
    if (!accel.begin()) {
        Serial.println("LSM303 não encontrado!");
        while(1);
    }
    // calibra offset em repouso
    sensors_event_t event;
    accel.getEvent(&event);
    offsetX = event.acceleration.x;
}

float lerAcelerometroX() {
    sensors_event_t event;
    accel.getEvent(&event);
    return event.acceleration.x - offsetX;
}

bool naRampa() {
    return abs(lerAcelerometroX()) > LIMIAR_RAMPA;
}