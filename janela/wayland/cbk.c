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

    bool primeiro_buffer = janela->buffer == NULL;
    janela->redraw_requested = 1;

    int resultado = create_shm_buffer(nos);
    if (resultado < 0)
        return;

    if (primeiro_buffer) {
        janela->buffer_liberado = 1;
        janela->frame_pronto = 1;
    }

    if (resultado == 0 && janela->frame_pronto && janela->buffer_liberado)
        attJanela(nos);
};

void registry_remover(void *data, struct wl_registry *registry, uint32_t id){

};

static int limpa_buffer(Janela *janela){
    if (janela->buffer) {
        wl_buffer_destroy(janela->buffer);
        janela->buffer = NULL;
    }

    if (janela->shm_data) {
        size_t size = (size_t)janela->stride * (size_t)janela->height;
        if (munmap(janela->shm_data, size) < 0) {
            perror("munmap do buffer Wayland");
            return -1;
        }
        janela->shm_data = NULL;
        janela->pixl = NULL;
    }

    if (janela->cls >= 0) {
        if (close(janela->cls) < 0) {
            perror("close do arquivo shm Wayland");
            janela->cls = -1;
            return -1;
        }
        janela->cls = -1;
    }

    janela->stride = 0;
    return 0;
}

int create_shm_buffer(void *data){

    if (!data) {
        fprintf(stderr, "create_shm_buffer: contexto nulo\n");
        return -1;
    }

    Nos *nos = data;
    if (!nos->janela) {
        fprintf(stderr, "create_shm_buffer: janela nula\n");
        return -1;
    }

    Janela *janela = nos->janela;
    sWayland *wayland = nos->wayland;
    int width = janela->novoWidth;
    int height = janela->novoHeight;

    if (!wayland || !wayland->shm) {
        fprintf(stderr, "create_shm_buffer: wl_shm indisponível\n");
        return -1;
    }
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "create_shm_buffer: tamanho inválido (%d x %d)\n", width, height);
        return -1;
    }
    if (janela->buffer && width == janela->width && height == janela->height)
        return 0;
    if (janela->buffer && !janela->buffer_liberado) {
        janela->redraw_requested = 1;
        return 1;
    }
    if ((uint32_t)width > UINT32_MAX / 4) {
        fprintf(stderr, "create_shm_buffer: largura excede o limite do stride\n");
        return -1;
    }

    uint32_t stride = (uint32_t)width * 4;
    if ((size_t)stride > SIZE_MAX / (size_t)height) {
        fprintf(stderr, "create_shm_buffer: tamanho do buffer excede SIZE_MAX\n");
        return -1;
    }
    size_t size = (size_t)stride * (size_t)height;
    if (size > INT32_MAX) {
        fprintf(stderr, "create_shm_buffer: tamanho excede o limite do protocolo Wayland\n");
        return -1;
    }

    char template[] = "/tmp/wayland-shm-XXXXXX";
    int fd = mkstemp(template);
    if (fd < 0) {
        perror("mkstemp para buffer Wayland");
        return -1;
    }
    unlink(template);

    if (ftruncate(fd, (off_t)size) < 0) {
        perror("ftruncate do buffer Wayland");
        close(fd);
        return -1;
    }

    void *shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("mmap do buffer Wayland");
        close(fd);
        return -1;
    }   

    struct wl_shm_pool *pool =
        wl_shm_create_pool(wayland->shm, fd, (int32_t)size);
    if (!pool) {
        fprintf(stderr, "create_shm_buffer: wl_shm_create_pool falhou\n");
        munmap(shm_data, size);
        close(fd);
        return -1;
    }

    struct wl_buffer *buffer =
        wl_shm_pool_create_buffer(pool, 0, width, height, stride, janela->pixel_format);

    wl_shm_pool_destroy(pool);
    if (!buffer) {
        fprintf(stderr, "create_shm_buffer: wl_shm_pool_create_buffer falhou\n");
        munmap(shm_data, size);
        close(fd);
        return -1;
    }
    if (wl_buffer_add_listener(buffer, &buffer_listener, janela) < 0) {
        fprintf(stderr, "create_shm_buffer: não foi possível registrar listener do buffer\n");
        wl_buffer_destroy(buffer);
        munmap(shm_data, size);
        close(fd);
        return -1;
    }

    if (limpa_buffer(janela) < 0) {
        wl_buffer_destroy(buffer);
        munmap(shm_data, size);
        close(fd);
        return -1;
    }

    janela->width = width;
    janela->height = height;
    janela->stride = stride;
    janela->cls = fd;
    janela->shm_data = shm_data;
    janela->pixl = shm_data;
    janela->buffer = buffer;

    return 0;
};

void handle_toplevel_configure(void *data,  struct xdg_toplevel *xdg_toplevel,  int32_t width, int32_t height,  struct wl_array *states){
    Nos *nos = data;

    /*
     * width/height podem ser acidentalmente 0, especialmente
     * quando o compositor não quer impor um tamanho.
     */
    /**/
    if (width > 0)
        nos->janela->novoWidth = width;

    if (height > 0)
        nos->janela->novoHeight = height;
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
    Janela *janela = data;

    if (buffer != janela->buffer)
        return;

    janela->buffer_liberado = 1;
    if (janela->novoWidth != janela->width || janela->novoHeight != janela->height) {
        int resultado = create_shm_buffer(janela->nos);
        if (resultado < 0)
            return;
    }

    if (janela->frame_pronto && janela->redraw_requested){
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
    if (janela->width != janela->novoWidth || janela->height != janela->novoHeight) {
        janela->redraw_requested = 1;
        return;
    }

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