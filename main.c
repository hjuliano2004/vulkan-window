#define _POSIX_C_SOURCE 200809L
#include "janela/Janela.h"
#include "janela/wayland/cbk.h"
#include "janela/xdg-shell-client-protocol.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wayland-client.h>

#include "tempo/Diferenca.h"
#include "tempo/Setts.h"

#define padrao 9

void imagemTeste(void *data) {

    Nos *nos = data;

    Janela *janela = nos->janela;

    const uint32_t x = 0;//0xFFFFFFFF; //branco
    const uint32_t o =  0xFF000000; //preto
   

uint32_t pixels[padrao][padrao] = {
        {x, x, x, x, o, x, x, x, x},
        {x, x, x, o, o, o, x, x, x},
        {x, x, x, o, x, o, x, x, x},
        {x, x, o, o, x, o, o, x, x},
        {x, x, o, o, o, o, o, x, x},
        {x, o, o, x, x, x, o, x, x},
        {x, o, o, x, x, x, o, o, x},
        {x, o, x, x, x, x, x, o, x},
        {o, o, x, x, x, x, x, o, o}
    };

/*
{
                                    {x, o, o, x, x, x, o, o, x},
                                    {x, o, o, x, x, x, o, o, x},
                                    {x, o, o, x, x, x, o, o, x},
                                    {x, o, o, x, x, x, o, o, x},
                                    {x, o, o, o, o, o, o, o, x},
                                    {x, o, o, o, o, o, o, o, x},
                                    {x, o, o, x, x, x, o, o, x},
                                    {x, o, o, x, x, x, o, o, x},
                                    {x, o, o, x, x, x, o, o, x}
};*/

uint32_t *data_shm = janela->shm_data;

        for(int i = 0; i < padrao && i < janela->height; i++){
            for(int j = 0; j < padrao && j < janela->width; j++){
                if(pixels[i][j]){
                    data_shm[i * janela->width + j] = pixels[i][j];
                }
            }
        }



solicitarRedesenho(janela);
}

void spam(void *arg);

int main(void) {

    Janela *janela = newJanela();
    sWayland *wayland = newWayland();
    Nos *nos = newNos(janela, wayland, "janela de teste");

    if (!janela || !wayland || !nos)
        return 1;


    uint32_t *pixel = (uint32_t *)janela->shm_data;
    for (int i = 0; i < janela->width * janela->height; ++i) {
        pixel[i] = 0xFFFFFFFF;
    }

    solicitarRedesenho(janela);

    double FPS = 0; // s/60;

    //setInterval(spam, janela, 0.3);
    setTimeOut(imagemTeste, nos, 0.3);

    while (janela->cls != 0) {
        wl_display_dispatch_pending(wayland->display);
        // wl_display_dispatch(wayland->display);
        wl_display_flush(wayland->display);

        controleCiclo(wayland, janela, FPS);

        calculoFps();
        rodar();
    }

    delJanela(janela);
    deslWayland(wayland);

    return 0;
}

void spam(void *arg) {
    Janela *janela = arg;

    // O timer pede um novo desenho; a escrita ocorre no callback de frame.
    solicitarRedesenho(janela);
    desenharJanela(janela);
    printf("mensagem a cada 3 segundos\n");
    printf("FPS: %.0f\n", ciclo->FPS);
}

/*


cd 'Área de trabalho'
cd vulkan

*/