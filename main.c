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

void spam(void *arg);

int main(void) {

    Janela *janela = newJanela();
    sWayland *wayland = newWayland();
    Nos *nos = newNos(janela, wayland, "janela de teste");


    uint32_t *pixel = (uint32_t *)janela->shm_data;
    for (int i = 0; i < janela->width * janela->height; ++i) {
        pixel[i] = 0xFFFFFFFF;
    }

    solicitarRedesenho(janela);

    double FPS = 0;

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