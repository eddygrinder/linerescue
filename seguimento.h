#pragma once

void seguimentoSetup();   // reset do PID
void seguirLinha(int erro);       // uma iteração do PID — chamar no loop()
void seguirLinhaSubida(int erro);
void seguirLinhaDescida(int erro);