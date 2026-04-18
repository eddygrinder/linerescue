#include "motores.h"
#include "sensores.h"
#include "seguimento.h"
#include "manobras.h"
#include "encoders.h"
#include "rgb.h"

// ─── ESTADOS ─────────────────────────────────────────────────────
enum Estado
{
  CALIBRAR,
  SEGUIR_LINHA,
  DESVIAR,      // reservado — VL53L0X
  SUBIR_RAMPA,  // reservado — acelerómetro
  DESCER_RAMPA, // reservado — acelerómetro
  ENTR_ESQ,     // reservado — sensores da esquerda veem linha e os da direita não
  ENTR_DTO,     // reservado — sensores da direita veem linha e os da esquerda não
  CRUZAMENTO,   // reservado — QTR padrão
  INVERSAO,     // reservado — ambos sensores veem verde,
  ATRAVESSAR,   // reservado — atravessa a linha às cegas até sair do preto
  PARAR,
  VERIFICAR_INTERSECAO, // reservado — entroncamento detectado, verificar se tem cor e decidir o que fazer
};

Estado estado = CALIBRAR;
bool verdeRecente = false;
bool ultimoPretoEsq = false; // para decidir virar para onde no entroncamento
bool ultimoPretoDto = false; // para decidir virar para onde no entroncamento

// ─── SETUP ───────────────────────────────────────────────────────
void setup()
{
  Serial.begin(115200);
  motorSetup();
  encoders_init();
  sensoresSetup();
  rgbSetup();
  delay(500);

  calibrarQTR();
  seguimentoSetup();

  estado = SEGUIR_LINHA;
  Serial.println("A seguir linha...");
  delay(1000);
}

/*
loop()
  │
  ├── 1. lerSensores()          → array sensor[] preenchido
  ├── 2. lerUltrasonico()       → distancia_cm preenchido
  │
  ├── 3. Pré-calcular tudo
  │       posicao, erro
  │       lado_do_verde()
  │       cruzamento_detectado()
  │
  └── 4. switch(estado)
            SEGUIR_LINHA   → PID com erro já calculado
                           → testa verde com leitura já feita
                           → testa cruzamento com leitura já feita
                           → testa obstáculo com distancia já lida
            CRUZAMENTO     → decide virar/seguir/360
            SUBIR_RAMPA    → controlo de velocidade
            DESVIAR        → lógica de desvio
            PARAR          → para
*/

// ─── LOOP ────────────────────────────────────────────────────────
void loop()
{
  qtr.readCalibrated(sensorValues);
  bool linha = linhaDetectada();

  static unsigned long tRGB = 0;
  if (millis() - tRGB > 200)
  { // lê RGB a cada 50ms
    rgbUpdate();
    tRGB = millis();
  }
  uint16_t posicao = 3500;
  int erro = 0;
  if (linha)
  {
    posicao = qtr.readLineBlack(sensorValues);
    erro = (int)posicao - 3500;
  }

  switch (estado)
  {
  case SEGUIR_LINHA:
    if (!linha)
    {
      static unsigned long tSemLinha = 0;
      if (tSemLinha == 0)
        tSemLinha = millis(); // marca quando perdeu a linha

      if (millis() - tSemLinha > 2000)
      { // 2s sem linha → para
        estado = PARAR;
        tSemLinha = 0;
        break;
      }
      // menos de 2s → anda em frente
      setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
      tSemLinha = 0; // tem linha → reset do timer
      if (entroncamentoEsq())
      {
        estado = ENTR_ESQ;
        break;
      }
      if (entroncamentoDir())
      {
        estado = ENTR_DTO;
        break;
      }
      seguirLinha(erro);
      break;
    }
    if (entroncamentoEsq())
    {
      estado = ENTR_ESQ;
    }
    if (entroncamentoDir())
    {
      estado = ENTR_DTO;
    }
    seguirLinha(erro);
    break;
  case ENTR_ESQ:
    resetEncoders();
    setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
    while (ticksMedio() < TICKS_CENTRO)
    {
    }
    pararMotores();
    delay(WAIT_VIRA_MS);
    virarEsquerda90();
    ignorarVerdePor(500);
    estado = SEGUIR_LINHA;
    break;
  case ENTR_DTO:
    resetEncoders();
    setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
    while (ticksMedio() < TICKS_CENTRO)
    {
    }
    pararMotores();
    delay(WAIT_VIRA_MS);
    virarDireita90();
    ignorarVerdePor(500);
    estado = SEGUIR_LINHA;
    break;
  case PARAR:
    pararMotores();
    break;
  }
}