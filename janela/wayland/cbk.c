#define _POSIX_C_SOURCE 200809L

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
#include "../xdg-shell-client-protocol.h"
#include "../Janela.h"
#include "cbk.h"


const struct xdg_wm_base_listener wm_base_listener = {
    .ping = wm_base_ping,
};

const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

const struct wl_callback_listener frame_listener;



const struct wl_registry_listener registry_listener = {
    registry_handler,
    registry_remover
};



void wm_base_ping(void *data, struct xdg_wm_base *wm_base, uint32_t serial) {
    xdg_wm_base_pong(wm_base, serial);
}


void registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version) {


    
    Nos *nos = data;
    sWayland *wayland = nos->wayland;

    if (strcmp(interface, "wl_compositor") == 0) {
        wayland->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shm") == 0) {
        wayland->shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        wayland->wm_base = wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(wayland->wm_base, &wm_base_listener, NULL);
    }
}

void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {

    Nos *nos = data;
    Janela *janela = nos->janela;

    xdg_surface_ack_configure(xdg_surface, serial);
    if (!janela->buffer) {
        if (create_shm_buffer(nos) == 0) {
            wl_surface_attach(janela->surface, janela->buffer, 0, 0);
            wl_surface_commit(janela->surface);
            /* Agendar primeiro frame callback para iniciar o loop de redraw */
            janela->frame_callback = wl_surface_frame(janela->surface);
            wl_callback_add_listener(janela->frame_callback, &frame_listener, nos);
        }
    }
}

void frame_done(void *data, struct wl_callback *callback, uint32_t time) {

    (void)time;
    Nos *nos = data;
    Janela *janela = nos->janela;

    /* Destrói callback antigo */
    if (callback) wl_callback_destroy(callback);
    janela->frame_callback = NULL;

    /* Aqui você pode atualizar pixels em janela->shm_data antes de anexar */
    if (janela->buffer) {
        wl_surface_attach(janela->surface, janela->buffer, 0, 0);
        wl_surface_commit(janela->surface);
    }

    /* Re-agenda próximo frame */
    janela->frame_callback = wl_surface_frame(janela->surface);
    wl_callback_add_listener(janela->frame_callback, &frame_listener, nos);
}

const struct wl_callback_listener frame_listener = {
    .done = frame_done,
};

int create_shm_buffer(void *data) {

    Nos *nos = data;
    Janela *janela = nos->janela;
    sWayland *wayland = nos->wayland;

    int stride = janela->width * 4;
    size_t size = stride * janela->height;
    char template[] = "/tmp/wayland-shm-XXXXXX";
    janela->cls = mkstemp(template);
    if (janela->cls < 0) return -1;
    unlink(template);
    if (ftruncate(janela->cls, size) < 0) return -1;
    janela->shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, janela->cls, 0);
    if (janela->shm_data == MAP_FAILED) return -1;

    /* Preenche branco (XRGB8888) */
    uint32_t *pixel = (uint32_t *)janela->shm_data;
    for (int i = 0; i < janela->width * janela->height; ++i) pixel[i] = 0x00FFFFFF;

    struct wl_shm_pool *pool = wl_shm_create_pool(wayland->shm, janela->cls, size);
    janela->buffer = wl_shm_pool_create_buffer(pool, 0, janela->width, janela->height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    return 0;
}

void registry_remover(void *data, struct wl_registry *registry, uint32_t id) {}