#ifndef DIFERENCA_H
#define DIFERENCA_H

#include <time.h>
#include <stdbool.h>
#include "Setts.h"

double decorrido(struct timespec tempo);
bool espera(double espera, struct timespec *inicio);

#endif
