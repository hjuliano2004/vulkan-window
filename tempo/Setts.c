#define _XOPEN_SOURCE 500
#include "Setts.h"
#include "Diferenca.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

TimerList *temporizadores = NULL;

static int nextId = 0;

SetTimer *setInterval(void (*callback)(void *arg), void *arg,  double intervalo) {

    if (temporizadores == NULL) {
        temporizadores = timerList();
    }

    SetTimer *setTimer = malloc(sizeof(SetTimer));
    setTimer->callback = callback;
    setTimer->arg = arg;
    setTimer->intervalo = intervalo;
    setTimer->id = nextId++;
    setTimer->repete = true;

    clock_gettime(CLOCK_MONOTONIC, &setTimer->inicio);

    addTimer(temporizadores, setTimer);

    return setTimer;
}

SetTimer *setTimeOut(void (*callback)(void *arg), void *arg,  double intervalo){

    SetTimer *setTimer = setInterval(callback, arg, intervalo);

    setTimer->repete = false;

    return setTimer;
}




TimerList *timerList() {
    int padrao = 50;
    TimerList *list = malloc(sizeof(TimerList));
    list->length = 0;
    list->size = padrao;
    list->list = malloc(padrao * sizeof(SetTimer *));

    for (int i = 0; i < padrao; i++) {
        list->list[i] = NULL;
    }

    return list;
}

TimerList *addTimer(TimerList *timerlist, SetTimer *setTimer) {
    if (timerlist->length == timerlist->size) {
        int novoTamanho = timerlist->size + (timerlist->size / 3);

        SetTimer **nova =
            realloc(timerlist->list, novoTamanho * sizeof(SetTimer *));

        if (nova == NULL) {
            return NULL;
        }
        timerlist->list = nova;

        timerlist->size = novoTamanho; // atualiza o tamanho
    }

    timerlist->list[timerlist->length] = setTimer;
    timerlist->length++;

    return timerlist;
}

void clearInterval(SetTimer* setTimer) {

    int id = setTimer->id;
    for (int i = 0; i < temporizadores->length; i++) {
        if (temporizadores->list[i]->id == id) {
            free(temporizadores->list[i]); // libera memória do timer
            // compacta a lista
            for (int j = i; j < temporizadores->length - 1; j++) {
                temporizadores->list[j] = temporizadores->list[j+1];
            }
            temporizadores->length--;
            break;
        }
    }
}

void clearAllIntervals() {
    for (int i = temporizadores->length - 1; i >= 0; i--) {
        clearInterval(temporizadores->list[i]);
    }
}


void rodar() {
    if(!temporizadores){
        return;
    }
   for (int i = 0; i < temporizadores->length; i++) {
    repetidor(temporizadores->list[i]);
}

}

void repetidor(SetTimer *timer) {
    if (espera(timer->intervalo, &timer->inicio)) {
        timer->callback(timer->arg);
        if(!timer->repete){
            clearInterval(timer);
        }

    }
}