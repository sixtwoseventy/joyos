#include <joyos.h>

int usetup() {
    return 0;
}

#define GEARBOX_MAX 500

uint16_t servo_home[6] = {374,0,0,0,0,0};
uint16_t servo_active[6] = {206,0,0,0,0,0};

bool dispenser_active[6] = {0,0,0,0,0,0};

int run_dispensers() {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        servo_set_pos(i, servo_home[i]);
    }


    bool last_lever = false;
    while(1) {
        bool cur_lever = (analog_read(8) < 500);

        if (cur_lever && !last_lever) {
            for (i = 0; i < 6; i++) {
                if (dispenser_active[i]) {
                    servo_set_pos(i, servo_active[i]);
                }
            }
            pause(300);
            
            for (i = 0; i < 6; i++) {
                servo_set_pos(i, servo_home[i]);
            }
            pause(400);
        }

        last_lever = cur_lever;
    }


    return 0;
}

uint8_t quadrature_pin[6] = {0,2,4,6,24,26};

int run_gearboxes() {
    /*
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
    */

    // FOR DEMO ONLY:
    while(1) {
        if (go_press()) {
            encoder_reset(24);
        }
        if (encoder_read(24) > 50) {
            digital_write(0,1);
            dispenser_active[0] = true;
        } else {
            digital_write(0,0);
            dispenser_active[0] = false;
        }
        pause(100);
    }
    return 0;
}

int run_serial_mon() {
    char v;
    while(1) {
        scanf("%c", &v);
        printf("GOT: %d\n", v);
        pause(1000);
        //yield();
    }
    return 0;
}

int umain() {
    create_thread(&run_dispensers, STACK_DEFAULT, 0, "dispenser_thread");
//    create_thread(&run_serial_mon, STACK_DEFAULT, 0, "serial_mon_thread");
    run_gearboxes();
    return 0;
}
