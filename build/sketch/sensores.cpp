#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\sensores.cpp"
#include "sensores.h"
#include "motores.h"
#include <Arduino.h>
#include "manobras.h"

QTRSensors qtr;
uint16_t sensorValues[NUM_SENSORS];

#define LIMIAR_VERDE_LOW 250
#define LIMIAR_VERDE_HIGH 1000
#define LIMIAR_PRETO_RAW 2500

static const uint8_t SENSOR_PINS[NUM_SENSORS] = {A7, A6, A5, A4, A3, A2, A1, A0};
/*
peso 0    → esquerda  → pino do sensor mais à esquerda primeiro
peso 7000 → direita   → pino do sensor mais à direita por último
*/

void sensoresSetup()
{
  analogReadResolution(12);
  qtr.setTypeAnalog();
  qtr.setSensorPins(SENSOR_PINS, NUM_SENSORS);
}

void calibrarQTR()
{
  Serial.println("A calibrar... move o robot sobre a linha durante 5s");

  unsigned long tFim = millis() + 5000;
  while (millis() < tFim)
  {
    qtr.calibrate(); // ← chama continuamente durante 5s
    delay(10);
  }

  pararMotores();

  Serial.print("Min: ");
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Max: ");
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("Calibração concluída!");
}

uint16_t lerPosicaoLinha()
{
  return qtr.readLineBlack(sensorValues);
}

bool linhaDetectada()
{
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    if (sensorValues[i] > 500)
      return true;
  }
  return false;
}

bool leituraSensoresValida()
{
  qtr.read(sensorValues);
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    if (sensorValues[i] > 100)
      return true;
  }
  return false;
}

bool sensorEVerde(int idx)
{
  return sensorValues[idx] > LIMIAR_VERDE_LOW &&
         sensorValues[idx] < LIMIAR_VERDE_HIGH;
}

bool cruzamento_detectado()
{
  int pretos = 0;
  for (int i = 0; i < NUM_SENSORS; i++)
    if (sensorValues[i] > LIMIAR_PRETO)
      pretos++;
  return pretos >= 6; // afinar este valor
}

// No teu array {A7..A0}, esquerda física = índices 4-7
bool verdeEsquerda()
{
  int count = 0;
  for (int i = 4; i < 8; i++)
    if (sensorEVerde(i))
      count++;
  return count >= 3; // pelo menos 2 sensores esquerdos no verde
}

bool verdeDireita()
{
  int count = 0;
  for (int i = 0; i < 4; i++)
    if (sensorEVerde(i))
      count++;
  return count >= 2;
}

// qualquer padrão com muitos pretos = interseção
bool intersecaoDetectada()
{
  int pretos = 0;
  for (int i = 0; i < NUM_SENSORS; i++)
    if (sensorValues[i] > LIMIAR_PRETO)
      pretos++;
  return pretos >= 4; // metade ou mais
}

bool todosBrancos()
{
  for (int i = 0; i < NUM_SENSORS; i++)
    if (sensorValues[i] < LIMIAR_PRATA)
      return true;
  return false;
}

bool entroncamentoEsq()
{
  return sensorValues[6] > LIMIAR_PRETO && sensorValues[7] > LIMIAR_PRETO;
}

bool entroncamentoDir()
{
  return sensorValues[0] > LIMIAR_PRETO && sensorValues[1] > LIMIAR_PRETO;
}

bool curvaOuEntroncamento()
{
  qtr.readCalibrated(sensorValues);

  bool centraisPretos = sensorValues[3] > 600 || sensorValues[4] > 600;
  bool pontasBrancos = sensorValues[0] < LIMIAR_PRETO && sensorValues[7] < LIMIAR_PRETO;

  if (centraisPretos && pontasBrancos)
    return false; // entroncamento → segue em frente
  if (!centraisPretos && pontasBrancos)
    return true; // curva → vira 90°
  return false;  // dúvida → segue em frente
}