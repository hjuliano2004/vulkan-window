#define _POSIX_C_SOURCE 199309L
#include "Diferenca.h"
#include "Setts.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Ciclo *ciclo = NULL;
double segundo = 1;

double decorrido(struct timespec *inicio) { // calcula e retorna o tempo de
                                           // espera

    struct timespec fim;
    clock_gettime(CLOCK_MONOTONIC, &fim);

    time_t sec = fim.tv_sec - inicio->tv_sec;
    long nsec = fim.tv_nsec - inicio->tv_nsec;

    if (nsec < 0) {
        sec -= 1;
        nsec += 1000000000;
    }

    return sec + nsec / 1e9;
}

bool espera(double espera, struct timespec *inicio) {

    double esperei = decorrido(inicio);

    if (esperei >= espera) {
        clock_gettime(CLOCK_MONOTONIC, inicio);
        return true;
    }

    return false;
}

void calculoFps(){//calcula o fps em tempo real, não serve pra definir fps fixo
    if(!ciclo){
        ciclo = malloc(sizeof(Ciclo));
        ciclo->ultimoFrame = malloc(sizeof(struct timespec));
        clock_gettime(CLOCK_MONOTONIC, ciclo->ultimoFrame);
    }

    ciclo->FPS =  segundo / decorrido(ciclo->ultimoFrame);
    clock_gettime(CLOCK_MONOTONIC, ciclo->ultimoFrame);
}


/*typedef struct {
    double FPS;
    struct timespec ultimoFrame;
}Ciclo;
*/