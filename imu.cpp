#include "imu.h"
#include "config.h"
#include <Adafruit_LSM303_U.h>

static Adafruit_LSM303_Mag_Unified mag(12345);
static Adafruit_LSM303_Accel_Unified accel(54321);
static float offsetX = 0;

float lerAcelerometroX() {
    sensors_event_t event;
    accel.getEvent(&event);
    return event.acceleration.x - offsetX;
}

bool naRampa() {
    return abs(lerAcelerometroX()) > LIMIAR_RAMPA;
}

void imuSetup() {
    // acelerómetro já existente...
    if (!accel.begin()) {
        Serial.println("LSM303 accel não encontrado!");
        while(1);
    }
    // magnetómetro
    if (!mag.begin()) {
        Serial.println("LSM303 mag não encontrado!");
        while(1);
    }
    sensors_event_t event;
    accel.getEvent(&event);
    offsetX = event.acceleration.x;
}

float getHeading() {
    sensors_event_t event;
    mag.getEvent(&event);
    float heading = atan2(event.magnetic.y, event.magnetic.x) * 180.0 / PI;
    if (heading < 0) heading += 360;
    return heading;
}