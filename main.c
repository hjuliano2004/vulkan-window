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
#include <poll.h>

#define s 1000

// callback precisa ter a mesma assinatura que o setInterval espera
void spam(void *arg);

int main(void) {

    Janela *janela = newJanela();
    sWayland *wayland = newWayland();
    Nos *nos = newNos(janela, wayland, "janela de teste");

    printf("janela cls: %d\n", janela->cls);

    double FPS = s / 60;

    // passa NULL como argumento, já que não precisa
    setInterval(spam, janela, 3);
    //setTimeOut(spam, 0, 9);

             

    /* Loop principal: processa eventos pendentes, timers e aguarda eventos com timeout */
    while (janela->cls != 0) {
        wl_display_dispatch_pending(wayland->display);
        wl_display_flush(wayland->display);

        if(controleCiclo(wayland, s)){break;}


         rodar();
    }


delJanela(janela);
deslWayland(wayland);



    return 0;
}

void spam(void *arg) {
    (void)arg; // evita warning de argumento não usado

    Janela *janela = arg;

    janela->cls = 0;
    printf("mensagem a cada 3 segundos\n");
}



/*


cd 'Área de trabalho'
cd vulkan

*/