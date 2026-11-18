#ifndef TECLADO_H
#define TECLADO_H

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

struct keyboard_state {
    struct wl_keyboard *keyboard;

    struct xkb_context *xkb_context;
    struct xkb_keymap *keymap;
    struct xkb_state *xkb_state;
};

extern const struct wl_keyboard_listener keyboard_listener;


void keyboard_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size);
void keyboard_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state_w);
void keyboard_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group);
void keyboard_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);
void keyboard_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface);

#endif