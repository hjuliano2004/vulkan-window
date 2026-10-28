#define _POSIX_C_SOURCE 200809L
#include "janela/Janela.h"
#include "janela/wayland/cbk.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client.h>

#include "tempo/Setts.h"


void redimensionar(void *arg){

    Nos *nos = arg;

    nos->janela->novoWidth = 1920;
    create_shm_buffer(nos);
    //solicitarRedesenho(nos->janela);
     attJanela(nos->janela);
}

int main(void) {

    Janela *janela = new_Janela("janela de teste");
    Janela *janela2 = new_Janela("janela 2");
    sWayland *wayland = new_Wayland();
    Nos *nos = new_Nos(janela, wayland);
    new_Nos(janela2, wayland);


    uint32_t *pixel = (uint32_t *)janela->shm_data;
    for (int i = 0; i < janela->width * janela->height; ++i) {
        pixel[i] = WHITE;
    }

    FILE *f = fopen("rubi.ppm", "r");


    if(!f){
        perror("erro ao abrir arquivo");
        return 1;
    }

    setTimeOut(redimensionar, nos, 2);//testando redimensionamento instantaneo

    

    //double FPS = 1000/60;

    attJanela(janela);
    attJanela(janela2);
    while (janela->cls != 0) {
        wl_display_dispatch_pending(wayland->display);
        // wl_display_dispatch(wayland->display);
        wl_display_flush(wayland->display);

        rodar();
        //liberaLista();
    }

    //delJanela(janela);
    //delJanela(janela2);

    //liberaLista();//destroi lista de janelas criadas
    //deslWayland(wayland);

    return 0;
}