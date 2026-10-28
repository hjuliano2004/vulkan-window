#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>
#include "../Janela.h"

typedef struct {
    double x;
    double y;

    uint32_t ultimo_serial;

    bool botao_esquerdo;
    bool botao_direito;
    struct wl_surface *surface;
} Mouse;

extern Mouse *mouse;
extern struct wl_pointer *pointer;

void new_Mouse(struct wl_seat *seat);
void pointer_motion(void *data, struct wl_pointer *pointer,   uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
void pointer_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

void pointer_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
void pointer_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
void pointer_axis(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
Janela *janela_alvo(Mouse *mouse);








#endif


/*
BTN_LEFT   = 0x110
BTN_RIGHT  = 0x111
BTN_MIDDLE = 0x112
*/