#include "Mouse.h"

#include <wayland-client-protocol.h>
#include <linux/input-event-codes.h>
#include <stdlib.h> 
#include <stddef.h> 
#include <stdio.h>
#include "../../janela/Janela.h"
#include "../protocolo/xdg-shell-client-protocol.h"

const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis,
};

Mouse *mouse = NULL;
struct wl_pointer *pointer = NULL;

void new_Mouse(struct wl_seat *seat, Janela *janela){

    if(seat == NULL){
        printf("seat nullo em new mouse\n");
        return;
    }

    if(!mouse){
        mouse = malloc(sizeof(Mouse));
         mouse->janela = janela;//TODO: remover janela unica futuramente
    }
    if(!pointer){
        pointer = wl_seat_get_pointer(seat);
    }
   
};

void pointer_motion(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy){

    Mouse *ponteiro = data;
    ponteiro->x = wl_fixed_to_double(sx);
    ponteiro->y = wl_fixed_to_double(sy);

    // printf("x: %f, y: %f\n", mouse->x, mouse->y);
}



static void direita(uint32_t button, uint32_t state){//eventos clique direito
    if (button == BTN_RIGHT && (state == WL_POINTER_BUTTON_STATE_PRESSED)){
        mouse->botao_direito = (state == WL_POINTER_BUTTON_STATE_PRESSED);
        // printf("clique direito\n");
    }

    if (button == BTN_RIGHT && (state == WL_POINTER_BUTTON_STATE_RELEASED)){
        mouse->botao_direito = (state == WL_POINTER_BUTTON_STATE_RELEASED);
        // printf("soltar direito\n");
    }

}

static void esquerda(uint32_t button, uint32_t state, uint32_t serial, Janela *janela){//eventos clique esquerdo

    if (button == BTN_LEFT && (state == WL_POINTER_BUTTON_STATE_PRESSED)){
        mouse->botao_esquerdo = (state == WL_POINTER_BUTTON_STATE_PRESSED);
        //mouse->ultimo_serial = serial;
        
        if(mouse->y < 30){
            xdg_toplevel_move(janela->toplevel, janela->nos->wayland->seat, serial);
        }
        //printf("x: %f, y: %f\n", mouse->x, mouse->y);
    }

    if (button == BTN_LEFT && (state == WL_POINTER_BUTTON_STATE_RELEASED)){
        mouse->botao_esquerdo = (state == WL_POINTER_BUTTON_STATE_RELEASED);
        // printf("soltar esquerdo\n");
    }
}

void pointer_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state){
    Mouse *mouse = data;

    mouse->ultimo_serial = serial;

    direita(button, state);
    esquerda(button, state, serial, mouse->janela);
}

void pointer_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y){
    Mouse *mouse = data;

    mouse->dentro = true;

    mouse->surface = surface;
}

void pointer_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface){
    Mouse *mouse = data;

    mouse->dentro = false;
    mouse->surface = NULL;
}

void pointer_axis(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value){
//TODO: ficará vazio até eu descobrir a necessidade de fazer algo nessa situação
}

/*
Janela *janela_alvo(Mouse *mouse){//em caso de existir mais de uma janela, essa função aponta qual janela foi clicada

   
    for(int i=0;i<listaJanelas->length;i++){
        if(listaJanelas->janelas[i]->surface == mouse->surface){

            return listaJanelas->janelas[i];
        }
    }

    printf("superifcie desconhecida\n");
    return NULL;
    
}*/


//void moveJanela(Nos *nos){

    //xdg_toplevel_move(nos->janela->toplevel, nos->wayland->seat, mouse->ultimo_serial);
//}