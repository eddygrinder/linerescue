#line 1 "C:\\Users\\ADMIN\\Documents\\GitHub\\linerescue\\seguimento.h"
#pragma once

void seguimentoSetup();   // reset do PID
void seguirLinha(int erro);       // uma iteração do PID — chamar no loop()