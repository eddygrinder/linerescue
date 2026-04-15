#pragma once
#include "config.h"

void motorSetup();
void setMotor(int pwm1, int pwm2, int en, int enb, int vel);
void setAllMotors(int fl, int fr, int rl, int rr);
void pararMotores();
void spinEsquerda(int vel);
void lateralEsquerda(int vel);
void lateraldireita(int vel);