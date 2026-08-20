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
#include <poll.h>


struct pollfd *pfd = NULL;

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
    janela->width = 0;
    janela->height = 0;
    janela->novoWidth = 600;
    janela->novoHeight = 400;


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
    janela->frame_callback = NULL;
    janela->buffer_liberado = 0;
    janela->frame_pronto = 0;
    janela->redraw_requested = 0;
    janela->nos = NULL;
    janela->pixel_format = WL_SHM_FORMAT_ARGB8888;

    return janela;
}


void delJanela(Janela *janela) {
    if (!janela) {
        printf("janela vazio no delJanela");
        return;
    }
    if (janela->frame_callback)
        wl_callback_destroy(janela->frame_callback);
    if (janela->buffer)
        wl_buffer_destroy(janela->buffer);
    if (janela->shm_data)
        munmap(janela->shm_data, (size_t)janela->stride * janela->height);
    if (janela->cls >= 0)
        close(janela->cls);
    xdg_toplevel_destroy(janela->toplevel);
    xdg_surface_destroy(janela->xdg);
    wl_surface_destroy(janela->surface);
    free(janela);
}

void deslWayland(sWayland *wayland) {
     wl_display_disconnect(wayland->display);
     }





Nos *newNos(Janela *janela, sWayland *wayland, char *titulo){
    Nos *nos = malloc(sizeof(Nos));

    nos->janela = janela;
    nos->wayland = wayland;
    janela->nos = nos;

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
    wl_display_roundtrip(wayland->display);

    return nos;

}

struct pollfd *gPfd(sWayland *wayland) {//gerar pfd e entregar 
    struct pollfd *pfd = calloc(1, sizeof(struct pollfd));

    if(!pfd){
        printf("não foi possível gerar pfd");
        return NULL;
    }

    pfd->fd = wl_display_get_fd(wayland->display);
    pfd->events = POLLIN;
    return pfd;
}

void controleCiclo(sWayland *wayland, Janela *janela, int miliseconds) {

    if(pfd){
            int ret = poll(pfd, 1, miliseconds);

    if (ret < 0) {
        janela->cls = 0;
    }

    if (ret > 0) {
        if (pfd->revents & POLLIN) {
            if (wl_display_dispatch(wayland->display) == -1) {
                janela->cls = 0; // erro no dispatch
            }
        }
    }

    }else{
        pfd = gPfd(wayland);
    }
}




/*            int timeout_ms = 16; // tempo de  espera em milisegundos
        int ret = poll(&pfd, 1, timeout_ms);
        if (ret < 0) break;
        if (ret > 0) {
            if (pfd.revents & POLLIN) {
                if (wl_display_dispatch(wayland->display) == -1) break;
            }
        }*/





/*TODO:  esses  comandos um pos o outro renderizam a janela sem travar o loop

        wl_display_roundtrip(wayland->display);
        wl_display_dispatch_pending(wayland->display);
        wl_display_flush(wayland->display);
        */