#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\motores.cpp"
#include "motores.h"
#include <Arduino.h>

void motorSetup() {
  int pinos[] = {
    FL_PWM1, FL_PWM2, FL_EN, FL_ENB,
    FR_PWM1, FR_PWM2, FR_EN, FR_ENB,
    RL_PWM1, RL_PWM2, RL_EN, RL_ENB,
    RR_PWM1, RR_PWM2, RR_EN, RR_ENB
  };
  for (int p : pinos) pinMode(p, OUTPUT);

  digitalWrite(FL_EN, HIGH); digitalWrite(FL_ENB, LOW);
  digitalWrite(FR_EN, HIGH); digitalWrite(FR_ENB, LOW);
  digitalWrite(RL_EN, HIGH); digitalWrite(RL_ENB, LOW);
  digitalWrite(RR_EN, HIGH); digitalWrite(RR_ENB, LOW);
}

void setMotor(int pwm1, int pwm2, int en, int enb, int vel) {
  if      (vel > 0) { analogWrite(pwm1, vel);   analogWrite(pwm2, 0);    }
  else if (vel < 0) { analogWrite(pwm1, 0);     analogWrite(pwm2, -vel); }
  else              { analogWrite(pwm1, 0);     analogWrite(pwm2, 0);    }
}

void setAllMotors(int fl, int fr, int rl, int rr) {
  setMotor(FL_PWM1, FL_PWM2, FL_EN, FL_ENB, fl);
  setMotor(FR_PWM1, FR_PWM2, FR_EN, FR_ENB, fr);
  setMotor(RL_PWM1, RL_PWM2, RL_EN, RL_ENB, rl);
  setMotor(RR_PWM1, RR_PWM2, RR_EN, RR_ENB, rr);
}

void pararMotores() {
  setAllMotors(0, 0, 0, 0);
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