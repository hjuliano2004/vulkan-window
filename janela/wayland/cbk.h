#ifndef CBK_H
#define CBK_H

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


extern const struct xdg_wm_base_listener wm_base_listener;
extern const struct xdg_surface_listener xdg_surface_listener;
extern const struct wl_registry_listener registry_listener;


void wm_base_ping(void *data, struct xdg_wm_base *wm_base, uint32_t serial);
void registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial);
void registry_remover(void *data, struct wl_registry *registry, uint32_t id);
int create_shm_buffer(void *data);


#endif