#include <joyos.h>

int usetup() {return 0;}

int umain() {
    while (1) {
        printf("%d\n", analog_read(23));
        pause(frob_read_range(0,400));
    }
    return 0;
}
