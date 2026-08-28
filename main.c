#define _POSIX_C_SOURCE 200809L
#include "janela/Janela.h"
#include "janela/wayland/cbk.h"
#include "janela/xdg-shell-client-protocol.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client.h>

int main(void) {

    Janela *janela = new_Janela();
    sWayland *wayland = new_Wayland();
    new_Nos(janela, wayland, "janela de teste");


    uint32_t *pixel = (uint32_t *)janela->shm_data;
    for (int i = 0; i < janela->width * janela->height; ++i) {
        pixel[i] = WHITE;
    }

    solicitarRedesenho(janela);

    double FPS = 0;

    while (janela->cls != 0) {
        wl_display_dispatch_pending(wayland->display);
        // wl_display_dispatch(wayland->display);
        wl_display_flush(wayland->display);

        controleCiclo(wayland, janela, FPS);
    }

    delJanela(janela);
    deslWayland(wayland);

    return 0;
}