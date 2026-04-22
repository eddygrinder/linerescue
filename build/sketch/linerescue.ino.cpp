#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
#include <Arduino.h>
#include "motores.h"
#include "sensores.h"
#include "seguimento.h"
#include "manobras.h"
#include "encoders.h"
#include "rgb.h"
#include "imu.h"
#include "vl53.h"

// ─── ESTADOS ─────────────────────────────────────────────────────
enum Estado
{
    SEGUIR_LINHA,
    DESVIAR,
    SUBIR_RAMPA,
    DESCER_RAMPA,
    ENTR_ESQ,
    ENTR_DTO,
    VERIFICAR_INTERSECAO,
    PARAR
};
Estado estado;

bool verdeRecente = false;
bool ultimoPretoEsq = false; // para decidir virar para onde no entroncamento
bool ultimoPretoDto = false; // para decidir virar para onde no entroncamento

// ─── SETUP ───────────────────────────────────────────────────────
#line 30 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void setup();
#line 72 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void loop();
#line 30 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\linerescue.ino"
void setup()
{
    Serial.begin(115200);
    motorSetup();
    encoders_init();
    sensoresSetup();
    rgbSetup();
    imuSetup();
    vl53Setup();
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
    static unsigned long tSemLinha = 0;
    static unsigned long tBloqueio = 0;
    static uint32_t ticksAntes = 0;
    uint16_t posicao = 3500;
    int erro = 0;

    if (millis() - tRGB > 200)
    { // lê RGB a cada 50ms
        rgbUpdate();
        tRGB = millis();
    }

    if (millis() - tBloqueio > 1000)
    {
        if (linha && (ticksMedio() - ticksAntes) < 3)
        {
            // tenta transpor com velocidade de debris primeiro
            setAllMotors(VEL_TRANSPOR_DEBRIS, VEL_TRANSPOR_DEBRIS,
                         VEL_TRANSPOR_DEBRIS, VEL_TRANSPOR_DEBRIS);
            delay(300);

            if (!linhaDetectada())
            {
                // ainda bloqueado — pode ser rampa, aumenta velocidade
                setAllMotors(VEL_TRANSPOR_RAMPA, VEL_TRANSPOR_RAMPA,
                             VEL_TRANSPOR_RAMPA, VEL_TRANSPOR_RAMPA);
                delay(300);
                if (naRampa())
                    estado = SUBIR_RAMPA;
            }
        }
        ticksAntes = ticksMedio();
        tBloqueio = millis();
    }

    if (estado == SEGUIR_LINHA && obstaculoDetectado())
    {
        estado = DESVIAR;
    }

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
            if (tSemLinha == 0)
                tSemLinha = millis();
            if (millis() - tSemLinha > 2000)
            {
                estado = PARAR;
                tSemLinha = 0;
                break;
            }
            setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
            break;
        }
        tSemLinha = 0;

        // 1. verde primeiro
        if (verdeDecisaoCompleta())
        {
            if (verdeDuploDetectado())
            {
                tBloqueio = millis();      // ← adiciona
                ticksAntes = ticksMedio(); // ← adiciona
                fazer180();
                ignorarVerdePor(1000);
                estado = SEGUIR_LINHA;
                break;
            }
            else if (verdeESQDetectado())
            {
                pararMotores();
                estado = ENTR_ESQ;
                break;
            }
            else if (verdeDTODetectado())
            {
                pararMotores();
                estado = ENTR_DTO;
                break;
            }
        }
        // 2. entroncamento QTR
        if (entroncamentoEsq())
        {
            ultimoPretoEsq = true;
            resetEncoders();
            // antes de qualquer manobra
            tBloqueio = millis();
            ticksAntes = ticksMedio();
            setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
            while (ticksMedio() < TICKS_CENTRO)
            {
            }
            pararMotores();
            estado = VERIFICAR_INTERSECAO;
            break;
        }
        if (entroncamentoDir())
        {
            ultimoPretoEsq = false;
            resetEncoders();
            // antes de qualquer manobra
            tBloqueio = millis();
            ticksAntes = ticksMedio();
            setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
            while (ticksMedio() < TICKS_CENTRO)
            {
            }
            estado = VERIFICAR_INTERSECAO;
            break;
        }
        // 3. PID
        seguirLinha(erro);
        break;
    case VERIFICAR_INTERSECAO:
        if (curvaOuEntroncamento())
        {
            pararMotores();
            delay(WAIT_VIRA_MS);
            tBloqueio = millis();      // ← adiciona
            ticksAntes = ticksMedio(); // ← adiciona
            if (ultimoPretoEsq)
                virarEsquerda90();
            else
                virarDireita90();
        }
        estado = SEGUIR_LINHA;
        // seguirLinha(erro);
        break;
    case ENTR_ESQ:
        resetEncoders();
        // antes de qualquer manobra
        tBloqueio = millis();
        ticksAntes = ticksMedio();
        setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
        while (ticksMedio() < TICKS_VERDE_PARA_LINHA)
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
        // antes de qualquer manobra
        tBloqueio = millis();
        ticksAntes = ticksMedio();
        setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
        while (ticksMedio() < TICKS_VERDE_PARA_LINHA)
        {
        }
        pararMotores();
        delay(WAIT_VIRA_MS);
        virarDireita90();
        ignorarVerdePor(500);
        estado = SEGUIR_LINHA;
        break;
    case SUBIR_RAMPA:
        if (lerAcelerometroX() < -LIMIAR_RAMPA)
        {
            // X ficou negativo → passou o topo → está a descer
            estado = DESCER_RAMPA;
            break;
        }
        seguirLinhaSubida(erro);
        break;
    case DESCER_RAMPA:
        if (lerAcelerometroX() > -LIMIAR_RAMPA)
        {
            estado = SEGUIR_LINHA;
            break;
        }
        seguirLinhaDescida(erro);
        break;
    case DESVIAR:
    {
        static uint8_t fase = 0;

        switch (fase)
        {
        case 0: // desliza esquerda até não ver obstáculo + meio robot
            deslizarControladoEsq();
            delay(PAUSA_MOTORES_MS);
            fase = 1;
            break;
        case 1: // avança — testa linha
            resetEncoders();
            setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
            while (ticksMedio() < TICKS_DESVIO_FREN)
            {
                qtr.readCalibrated(sensorValues);
                if (linhaDetectada())
                {
                    pararMotores();
                    virarEsquerda90();
                    fase = 0;
                    estado = SEGUIR_LINHA;
                    return;
                }
            }
            pararMotores();
            delay(PAUSA_MOTORES_MS);
            fase = 2;
            break;

        case 2: // roda 90° dto
            resetEncoders();
            setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
            while (ticksMedio() < TICKS_MEIO_ROBOT)
            {
            } // ~8cm de folga
            pararMotores();
            delay(PAUSA_MOTORES_MS);
            virarDireita90();
            delay(PAUSA_MOTORES_MS);
            fase = 3;
            break;

        case 3: // desliza esq até livre + avança até linha + pivota esq
            deslizarControladoEsq();
            delay(PAUSA_MOTORES_MS);
            fase = 4;
            break;

        case 4: // avança testando linha — obstáculo do lado direito
            resetEncoders();
            setAllMotors(VEL_BASE, VEL_BASE, VEL_BASE, VEL_BASE);
            while (ticksMedio() < TICKS_DESVIO_FREN)
            {
                qtr.readCalibrated(sensorValues);
                if (linhaDetectada())
                {
                    pararMotores();
                    virarEsquerda90(); // linha à esq → enquadra
                    fase = 0;
                    estado = SEGUIR_LINHA;
                    return;
                }
            }
            pararMotores();
            delay(PAUSA_MOTORES_MS);
            fase = 5;
            break;
        case 5: // case 5: // roda 90° dto → testa obstáculo → desliza esq → avança até linha → pivota esq
            virarDireita90();
            delay(PAUSA_MOTORES_MS);
            desviarEAvancar(); // desliza esq até livre + avança até linha
            virarEsquerda90(); // enquadra com a linha
            fase = 0;
            estado = SEGUIR_LINHA;
            break;
        }
        break;
    }
    case PARAR:
        pararMotores();
        break;
    }
}
