#ifndef DIFERENCA_H
#define DIFERENCA_H

#include <time.h>
#include <stdbool.h>
#include "Setts.h"

typedef struct {
    double FPS;//guarda o valor real de frames de forma dinamica, não serve pra definir o fps
    struct timespec *ultimoFrame;
}Ciclo;

extern Ciclo *ciclo;
extern double segundo;

double decorrido(struct timespec *tempo);
bool espera(double espera, struct timespec *inicio);
void calculoFps();

#endif
