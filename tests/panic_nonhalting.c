#include <kern/global.h>
#include <board.h>
#include <kern/thread.h>
#include <math.h>
#include <gyro.h>
#include <board.h>
//#include <pid.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

int panicbreak (void) {
    for (;;) {
        printf("\npanicbreak!");
        pause(500);
    }

    return 0;
}

int usetup (void) {
    return 0;
}

int umain(void) {
    create_thread(&panicbreak, 64, 0, "panicbreak");

    while (1) {
        yield();
        panic("fail");
    }

    return 0;
}
