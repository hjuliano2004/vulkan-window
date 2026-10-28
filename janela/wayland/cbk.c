#include "cbk.h"
#include "../Janela.h"
#include <string.h>
#include "../mouse/Mouse.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


const struct xdg_wm_base_listener wm_base_listener = {
    .ping = wm_base_ping,
};

const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

const struct wl_registry_listener registry_listener = {
    registry_handler,
    registry_remover
};

const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release,
};

const struct xdg_toplevel_listener toplevel_listener = {
    .configure = handle_toplevel_configure,
    .close = handle_close,
};

const struct wl_callback_listener frame_listener = {
    .done = frame_done,
};


void wm_base_ping(void *data, struct xdg_wm_base *wm_base, uint32_t serial){
    xdg_wm_base_pong(wm_base, serial);
};

void registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version){

    sWayland *wayland = data;

    if (strcmp(interface, "wl_compositor") == 0) {
        wayland->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else 
    
    if (strcmp(interface, "wl_shm") == 0) {
        wayland->shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
    } else
    
    if (strcmp(interface, "xdg_wm_base") == 0) {
        wayland->wm_base = wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(wayland->wm_base, &wm_base_listener, NULL);
    }

    if (strcmp(interface, "wl_seat") == 0) {
        wayland->seat = wl_registry_bind(registry, id, &wl_seat_interface, 1);
}

};

void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial){

    Nos *nos = data;
    Janela *janela = nos->janela;

    xdg_surface_ack_configure(xdg_surface, serial);

    if (!janela->buffer) {
        if (create_shm_buffer(nos) == 0) {
            // O buffer nasce livre e o primeiro frame pode ser enviado agora.
            janela->buffer_liberado = 1;
            janela->frame_pronto = 1;
            janela->redraw_requested = 1;
            attJanela(nos);
        }
    }
};

void registry_remover(void *data, struct wl_registry *registry, uint32_t id){

};

static void limpa_buffer(Janela *janela){
    if (janela->shm_data) {
        munmap(janela->shm_data, (size_t)janela->stride * janela->height);
        janela->shm_data = NULL;
    }
}

static int ver_tamanho(Janela *janela){

    if (janela->novoWidth <= 0 || janela->novoHeight <= 0) {
        janela->cls = 0;
        printf("tamanho de janela inválido\n");
        return -1;
    }

    janela->width = janela->novoWidth;
    janela->height = janela->novoHeight;

    return 0;
}


int create_shm_buffer(void *data){

    Nos *nos = data;
    Janela *janela = nos->janela;
    sWayland *wayland = nos->wayland;
    
    limpa_buffer(janela);

    if(ver_tamanho(janela)!= 0){return -1;}

    janela->stride = (uint32_t)janela->width * 4;
    size_t size = (size_t)janela->stride * janela->height;
    char template[] = "/tmp/wayland-shm-XXXXXX";
    janela->cls = mkstemp(template);
    if (janela->cls < 0) {
        return -1;
    }
    unlink(template);
    if (ftruncate(janela->cls, size) < 0) {
        return -1;
    }
    janela->shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, janela->cls, 0);

    if (janela->shm_data == MAP_FAILED) {
        janela->shm_data = NULL;
        return -1;
    }   

    janela->pixl = janela->shm_data;

    struct wl_shm_pool *pool =
        wl_shm_create_pool(wayland->shm, janela->cls, size);
    janela->buffer =
        wl_shm_pool_create_buffer(pool, 0, janela->width, janela->height,
                                  janela->stride, janela->pixel_format);

    wl_shm_pool_destroy(pool);
    wl_buffer_add_listener(janela->buffer, &buffer_listener, janela);


    return 0;
};

void handle_toplevel_configure(void *data,  struct xdg_toplevel *xdg_toplevel,  int32_t width, int32_t height,  struct wl_array *states){
    Nos *nos = data;

    /*
     * width/height podem ser acidentalmente 0, especialmente
     * quando o compositor não quer impor um tamanho.
     */
    if (width > 0)
        nos->janela->width = width;

    if (height > 0)
        nos->janela->height = height;
};

void registry_global(void *data, struct wl_registry *registry, uint32_t name, const struct wl_interface *interface, uint32_t version){

    sWayland *wayland = data;

    if(strcmp(interface->name, wl_seat_interface.name) == 0){
        
        wayland->seat = wl_registry_bind(registry,  name, &wl_seat_interface, 1);
    }
};


void frame_done(void *data, struct wl_callback *callback, uint32_t time){
    (void)time;
    Nos *nos = data;
    Janela *janela = nos->janela;

     //Cada callback de frame é de uso único. 
    if (callback){
        wl_callback_destroy(callback);
    }

    janela->frame_callback = NULL;
    janela->frame_pronto = 1;

    // O frame só pode ser reapresentado quando o compositor liberou o buffer.
    if (janela->buffer && janela->buffer_liberado && janela->redraw_requested) {
        attJanela(nos);
    }
};


void buffer_release(void *data, struct wl_buffer *buffer){
    (void)buffer;
    Janela *janela = data;

    janela->buffer_liberado = 1;
    if (janela->frame_pronto && janela->redraw_requested){
        ///solicitarRedesenho(janela);
        attJanela(janela->nos);
    }
};

void handle_close(void *data, struct xdg_toplevel *xdg_toplevel){

    Nos *nos = data;
    nos->janela->aberto = false;

    printf("btn desligar janela pressionado\n");

};

void attJanela(void *data){
    Nos *nos = data;
    Janela *janela = nos->janela;

    if (!janela->buffer || !janela->buffer_liberado)
        return;

    // O callback precisa ser registrado antes do commit que ele acompanha.

    janela->frame_callback = wl_surface_frame(janela->surface);
    wl_callback_add_listener(janela->frame_callback, &frame_listener, nos);

   /*
    if(!janela->frame_callback){
        janela->frame_callback = wl_surface_frame(janela->surface);
        wl_callback_add_listener(janela->frame_callback, &frame_listener, nos);
    }*/

    janela->buffer_liberado = 0;
    janela->frame_pronto = 0;
    janela->redraw_requested = 0;

    wl_surface_attach(janela->surface, janela->buffer, 0, 0);
    wl_surface_damage(janela->surface, 0, 0, janela->width, janela->height);
    wl_surface_commit(janela->surface);
};

void solicitarRedesenho(Janela *janela) {
    janela->redraw_requested = 1;
    if (janela->buffer && janela->frame_pronto) {
        // desenharJanela(janela);
        attJanela(janela->nos);
    }
}