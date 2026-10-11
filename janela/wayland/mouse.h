#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>

typedef struct {
    double x;
    double y;

    uint32_t ultimo_serial;

    bool botao_esquerdo;
    bool botao_direito;
} Mouse;

extern Mouse *mouse;

void new_Mouse();
void pointer_motion(void *data, struct wl_pointer *pointer,   uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
void pointer_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);










#endif


/*
BTN_LEFT   = 0x110
BTN_RIGHT  = 0x111
BTN_MIDDLE = 0x112
*/