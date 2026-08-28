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
    int novoWidth;
    int novoHeight;

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
    int buffer_liberado; // indica que o compositor terminou de usar o buffer
    int frame_pronto; // indica que o callback de frame autorizou o próximo desenho
    int redraw_requested; // pedido de redesenho feito pela aplicação
    struct Nos *nos; // contexto usado para enviar o buffer ao Wayland
    uint32_t pixel_format;
} Janela;

typedef struct Nos {
    Janela *janela;
    sWayland *wayland;

}Nos;

void delJanela(Janela *janela);
void deslWayland(sWayland *wayland);
sWayland *new_Wayland();
Janela *new_Janela();
Nos *new_Nos(Janela *janela, sWayland *wayland, char *titulo);
struct pollfd *gPfd(sWayland *wayland);
void controleCiclo(sWayland *wayland, Janela *janela, int miliseconds);




// Formato: 0xAARRGGBB (Alpha, Red, Green, Blue)

#define RED     0xFFFF0000
#define GREEN   0xFF00FF00
#define BLUE    0xFF0000FF
#define WHITE   0xFFFFFFFF
#define BLACK   0xFF000000
#define YELLOW  0xFFFFFF00
#define CYAN    0xFF00FFFF
#define MAGENTA 0xFFFF00FF
#define GRAY    0xFF808080
#define ORANGE  0xFFFFA500
#define PURPLE  0xFF800080
#define BROWN   0xFFA52A2A




#endif