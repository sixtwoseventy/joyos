#include <joyos.h>

int usetup() {
    return 0;
}

#define GEARBOX_MAX 500

uint16_t servo_home[6] = {374,0,0,0,0,0};
uint16_t servo_active[6] = {206,0,0,0,0,0};

int run_dispensers() {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        servo_set_pos(i, servo_home[i]);
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
    }


    return 0;
}

uint8_t quadrature_pin[6] = {0,2,4,6,24,26};

int run_gearboxes() {
    int16_t value[6] = {0,0,0,0,0,0};
    while(1) {
        
        for (int i = 0; i < 6; i++) {
            // Add the quadrature delta
            value[i] += quadrature_read(quadrature_pin[i]);

            // bound the value
            if (value[i] > GEARBOX_MAX) {
                value[i] = GEARBOX_MAX;
            } else if (value[i] < -GEARBOX_MAX) {
                value[i] = -GEARBOX_MAX;
            }

            quadrature_reset(quadrature_pin[i]);
        }
        
        printf("Value: %d\n", value[0]);

        pause(100);
        yield();
    }
    return 0;
}

int umain() {
    create_thread(&run_dispensers, STACK_DEFAULT, 0, "dispenser_thread");

    run_gearboxes();
    return 0;
}
