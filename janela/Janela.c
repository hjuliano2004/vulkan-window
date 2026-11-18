#define _POSIX_C_SOURCE 200809L
#include "Janela.h"
#include <stdlib.h>
#include <stdio.h>
#include "protocolo/xdg-shell-client-protocol.h"
#include <wayland-client.h>
#include <stdio.h>
#include <unistd.h>
#include "wayland/cbk.h"
#include "mouse/Mouse.h"

ListaJanelas *listaJanelas = NULL;


Janela *new_Janela(char *titulo){

    Janela *janela = calloc(1, sizeof(Janela));

    janela->aberto = true;

    janela->title = titulo;

    janela->novoWidth = 720; //1920
    janela->novoHeight = 480;//1080

    janela->pixel_format = WL_SHM_FORMAT_XRGB8888;

    janela->cls = -1;
    janela->maximized  = 0;
    janela->fullscreen = 0;
    janela->stride     = 0;

    janela->buffer_liberado = 0;
    janela->frame_pronto = 0;
    janela->redraw_requested = 0;

    addJanela(janela);

    return janela;

};

sWayland *new_Wayland(){

    sWayland *wayland = calloc(1, sizeof(sWayland));

    wayland->display = wl_display_connect(NULL);

    if(!wayland->display){
        printf("\n\nErro: não foi possível conectar ao servidor Wayland.\n\n");

        free(wayland);
        return NULL;
    }

    return wayland;
}

static int incrementos(Nos *nos){

    Janela *janela = nos->janela;
    sWayland *wayland = nos->wayland;

    wayland->registry = wl_display_get_registry(wayland->display);

    wl_registry_add_listener(wayland->registry, &registry_listener, wayland);//TODO: se der erro, voltar a usar wayland como argumento
    wl_display_roundtrip(wayland->display);

    if (!wayland->compositor || !wayland->shm || !wayland->wm_base) {
        fprintf(stderr, "Erro: interfaces Wayland não encontradas (compositor/shm/xdg_wm_base).\n");
        return 0;
    }

    janela->surface = wl_compositor_create_surface(wayland->compositor);
    janela->xdg = xdg_wm_base_get_xdg_surface(wayland->wm_base, janela->surface);
    janela->toplevel = xdg_surface_get_toplevel(janela->xdg);//cria a camada de gerenciamento da janela (barra de título, estados, etc)
    xdg_toplevel_set_title(janela->toplevel, janela->title);//titulo na barra de tarefas
    xdg_toplevel_set_app_id(janela->toplevel, janela->title);

    new_Mouse(wayland->seat, janela);
    int listener = xdg_toplevel_add_listener(janela->toplevel, &toplevel_listener, janela->nos);//adiciona listener para eventos da janela
    
    if(listener == -1){
        printf("\n\nlistener de eventos de janela recusado\n\n");
    }


    wl_pointer_add_listener(pointer, &pointer_listener, mouse);  //adiciona listeners do mouse

    return 1;
}

Nos *new_Nos(Janela *janela, sWayland *wayland){

    Nos *nos = calloc(1, sizeof(Nos));
    nos->janela = janela;
    nos->wayland = wayland;
    janela->nos = nos;

    if(!incrementos(nos)){return NULL;}

    xdg_surface_add_listener(janela->xdg, &xdg_surface_listener, nos);//adiciona listener pra eventos de configuração da janela

    
    

    wl_surface_commit(janela->surface);
    wl_display_roundtrip(wayland->display);

    return nos;
};



//--------------------auxiliares--------------------

void delJanela(Janela *janela) {//limpa a referencia da janela
    if (!janela) {
        printf("janela vazio no delJanela");
        return;
    }
    if (janela->frame_callback)
        wl_callback_destroy(janela->frame_callback);
    if (janela->buffer)
        wl_buffer_destroy(janela->buffer);
    if (janela->shm_data)
        munmap(janela->shm_data, (size_t)janela->stride * janela->height);
    if (janela->cls >= 0)
        close(janela->cls);
    xdg_toplevel_destroy(janela->toplevel);
    xdg_surface_destroy(janela->xdg);
    wl_surface_destroy(janela->surface);
    free(janela);
}

void deslWayland(sWayland *wayland) {//desconecta o wayland
    wl_display_disconnect(wayland->display);
    free(wayland);
}

void iniciar_lista(){
        int inicial = 5;
        if(!listaJanelas){
            listaJanelas = malloc(sizeof(ListaJanelas));
            listaJanelas->janelas = calloc(inicial, sizeof(Janela *));

            listaJanelas->length = 0;
            listaJanelas->size = inicial;
    }

}


void addJanela(Janela *janela){

    if(!janela){
        printf("tentativa de adicionar janela nula à lista de janelas\n");

        return;
    }

    iniciar_lista();

    if(listaJanelas->length == listaJanelas->size){

        Janela **listaNova = malloc((listaJanelas->size + 1) * sizeof(Janela *));

        for(int i=0;i<listaJanelas->length;i++){
            listaNova[i] = listaJanelas->janelas[i];
        }

        free(listaJanelas->janelas);
        listaJanelas->janelas = listaNova;
        listaJanelas->size++;
    }

    listaJanelas->janelas[listaJanelas->length] = janela;
    listaJanelas->length++;
}


    void liberaLista(){
    if (!listaJanelas){
        printf("lista de janelas nula\n");
        return;
    }

    int novoLength = 0;

    for (int i = 0; i < listaJanelas->length; i++){
        Janela *janela = listaJanelas->janelas[i];

        if (janela->cls == 0){
            delJanela(janela);
        }else{
            listaJanelas->janelas[novoLength] = janela;
            novoLength++;
        }
    }

    for (int i = novoLength; i < listaJanelas->size; i++){
        listaJanelas->janelas[i] = NULL;
    }

    listaJanelas->length = novoLength;
}