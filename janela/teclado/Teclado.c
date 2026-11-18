#include "Teclado.h"

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>// instalar com sudo apt install libxkbcommon-dev
#include <sys/mman.h>
#include <unistd.h>


const struct wl_keyboard_listener keyboard_listener = {
    .keymap     = keyboard_keymap,
    .enter      = keyboard_enter,
    .leave      = keyboard_leave,
    .key        = keyboard_key,
    .modifiers  = keyboard_modifiers,
    .repeat_info = NULL
};




void keyboard_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size){
    struct keyboard_state *state = data;

    char *map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

    state->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    state->keymap = xkb_keymap_new_from_string(state->xkb_context, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);

    state->xkb_state = xkb_state_new(state->keymap);

    munmap(map_shm, size);
    close(fd);
}

void keyboard_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state_w){
   struct keyboard_state *state_ptr = data;

    if (state_w != WL_KEYBOARD_KEY_STATE_PRESSED)
        return;

    xkb_keycode_t code = key + 8;


    xkb_keysym_t sym = xkb_state_key_get_one_sym(state_ptr->xkb_state, code);


    char name[128];

    xkb_keysym_get_name(sym, name, sizeof(name));
    //printf("sym = %s\n", name);
}


void keyboard_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group){
    struct keyboard_state *state = data;

    xkb_state_update_mask(state->xkb_state, depressed, latched, locked, 0, 0, group);
}

void keyboard_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys){
    (void)data;
    (void)keyboard;
    (void)serial;
    (void)surface;
    (void)keys;
}

void keyboard_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
    (void)data;
    (void)keyboard;
    (void)serial;
    (void)surface;
}