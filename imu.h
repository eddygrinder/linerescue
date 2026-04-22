#pragma once
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

void imuSetup();
float lerAcelerometroX();
bool naRampa();
float getHeading();
