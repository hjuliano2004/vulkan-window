#include "Janela.h"
#define _POSIX_C_SOURCE 200809L
#include "xdg-shell-client-protocol.h"
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
#include "wayland/cbk.h"


sWayland *newWayland() {
    sWayland *wayland = malloc(sizeof(sWayland));

    wayland->registry = NULL;
    wayland->compositor = NULL;
    wayland->shm = NULL;
    wayland->wm_base = NULL;

    wayland->display = wl_display_connect(NULL);

    if (!wayland->display) {
        fprintf(stderr, "Erro: não foi possível conectar ao Wayland.\n");
        return NULL;
    }

    return wayland;
}

Janela *newJanela() {
    Janela *janela = malloc(sizeof(Janela));

    // Estado interno
    janela->width = 600;
    janela->height = 400;
    janela->stride     = 0;
    janela->pixl       = NULL;
    janela->shm_data   = NULL;
    janela->title      = NULL;
    janela->opacidade  = 0;
    janela->cls        = -1;   // -1 é comum pra indicar "sem arquivo válido"
    janela->maximized  = 0;
    janela->fullscreen = 0;

    // Objetos Wayland/xdg-shell
    janela->surface = NULL;
    janela->buffer  = NULL;
    janela->xdg     = NULL;
    janela->toplevel     = NULL;

    return janela;
}

Nos *newNos(Janela *janela, sWayland *wayland, char *titulo[]){
    Nos *nos = malloc(sizeof(Nos));

    nos->janela = janela;
    nos->wayland = wayland;

    wayland->registry = wl_display_get_registry(wayland->display);

    wl_registry_add_listener(wayland->registry, &registry_listener, nos);

    wl_display_roundtrip(wayland->display);


    if (!wayland->compositor || !wayland->shm || !wayland->wm_base) {
        fprintf(stderr, "Erro: interfaces Wayland não encontradas (compositor/shm/xdg_wm_base).\n");
        return NULL;
    }

    janela->surface = wl_compositor_create_surface(wayland->compositor);

    janela->xdg = xdg_wm_base_get_xdg_surface(wayland->wm_base, janela->surface);

    xdg_surface_add_listener(janela->xdg, &xdg_surface_listener, nos);

    janela->toplevel = xdg_surface_get_toplevel(janela->xdg);
    xdg_toplevel_set_title(janela->toplevel, titulo);
    wl_surface_commit(janela->surface);





    return nos;

}
