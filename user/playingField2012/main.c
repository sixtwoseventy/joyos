#include <joyos.h>

int usetup() {
    return 0;
}


uint16_t servo_home[6] = {374,0,0,0,0,0};
uint16_t servo_active[6] = {206,0,0,0,0,0};

int umain() {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        servo_set_pos(i, servo_home[i]);
    }
    pause(500);
    for (i = 0; i < 6; i++) {
        servo_disable(i);
    }

    while(1) {
        go_click();
        for (i = 0; i < 6; i++) {
            servo_set_pos(i, servo_active[i]);
        }
        pause(300);
        
        for (i = 0; i < 6; i++) {
            servo_set_pos(i, servo_home[i]);
        }
        pause(400);

        for (i = 0; i < 6; i++) {
            servo_disable(i);
        }
    }
    return 0;
}
