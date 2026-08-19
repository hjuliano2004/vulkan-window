#define _POSIX_C_SOURCE 200809L
#include "janela/xdg-shell-client-protocol.h"
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
#include "janela/Janela.h"
#include "janela/wayland/cbk.h"

#include "tempo/Setts.h"
#include "tempo/Diferenca.h"


#define s 1000

// callback precisa ter a mesma assinatura que o setInterval espera
void spam(void *arg);

int main(void) {

    Janela *janela = newJanela();
    sWayland *wayland = newWayland();
    Nos *nos = newNos(janela, wayland, "janela de teste");


    printf("janela cls: %d\n", janela->cls);

    double FPS =  0;// s/60;

    setInterval(spam, janela, 3);
    //setTimeOut(spam, 0, 9);


    while (janela->cls != 0) {
        wl_display_dispatch_pending(wayland->display);
        wl_display_flush(wayland->display);

        controleCiclo(wayland, janela, FPS);


        calculoFps();
        rodar();
    }


delJanela(janela);
deslWayland(wayland);


    return 0;
}

void spam(void *arg) {
    (void)arg; // evita warning de argumento não usado

    Janela *janela = arg;

    //janela->cls = 0;
    printf("mensagem a cada 3 segundos\n");
    printf("FPS: %.0f\n", ciclo->FPS);
}



/*


cd 'Área de trabalho'
cd vulkan

*/