#ifndef JANELA_H
#define JANELA_H

#define _POSIX_C_SOURCE 200809L
#include "xdg-shell-client-protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wayland-client.h>


extern struct pollfd *pfd;//responsável pelo polling

typedef struct sWayland {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shm *shm;
    struct xdg_wm_base *wm_base;
} sWayland;

typedef struct Janela {
    // Estado interno
    int width;     // largura da janela
    int height;    // altura da janela
    uint32_t stride;    // bytes por linha (width * 4, por exemplo)
    uint8_t *pixl;      // ponteiro para pixels na memória compartilhada
    void *shm_data;     // ponteiro mapeado do tmpfile
    char *title;        // título da janela
    uint8_t opacidade;  // transparência/opacidade
    int cls;            // descriptor do tmpfile (shm)
    uint8_t maximized;  // estado maximizado
    uint8_t fullscreen; // estado fullscreen

    // Objetos de protocolo Wayland/xdg-shell
    struct wl_surface *surface;  // superfície gráfica
    struct wl_buffer *buffer;    // buffer anexado à superfície
    struct xdg_surface *xdg;  // camada intermediária do protocolo
    struct xdg_toplevel *toplevel; // janela gerenciável (barra de título, estados)
    struct wl_callback *frame_callback; // callback de frame para redraw contínuo
} Janela;

typedef struct{
    Janela *janela;
    sWayland *wayland;

}Nos;

void delJanela(Janela *janela);
void deslWayland(sWayland *wayland);
sWayland *newWayland();
Janela *newJanela();
Nos *newNos(Janela *janela, sWayland *wayland, char *titulo);
struct pollfd *gPfd(sWayland *wayland);
int controleCiclo(sWayland *wayland, int miliseconds);





#endif