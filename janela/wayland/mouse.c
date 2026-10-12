#include "mouse.h"

#include <wayland-client-protocol.h>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

Mouse *mouse = NULL;
struct wl_pointer *pointer = NULL;

void new_Mouse(struct wl_seat *seat){
    if(!mouse){
        mouse = malloc(sizeof(Mouse));
    }
    if(!pointer){
        pointer = wl_seat_get_pointer(seat);
    }
};


void pointer_motion(void *data, struct wl_pointer *pointer,   uint32_t time, wl_fixed_t sx, wl_fixed_t sy) {

    Mouse *ponteiro = data;
    ponteiro->x = wl_fixed_to_double(sx);
    ponteiro->y = wl_fixed_to_double(sy);
}

void pointer_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state){
    Mouse *mouse = data;

    mouse->ultimo_serial = serial;

    if(button == BTN_LEFT && (state == WL_POINTER_BUTTON_STATE_PRESSED)){
        mouse->botao_esquerdo = (state == WL_POINTER_BUTTON_STATE_PRESSED);
        printf("clique esquerdo\n");
    }
    if(button == BTN_RIGHT){
        mouse->botao_direito = (state == WL_POINTER_BUTTON_STATE_PRESSED);
    }
}
        /*
        valores possiveis
        WL_POINTER_BUTTON_STATE_RELEASED
        WL_POINTER_BUTTON_STATE_PRESSED*/