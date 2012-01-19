#include <joyos.h>

extern volatile uint8_t robot_id;

int usetup() {
    robot_id = 0;
    return 0;
}

#define GEARBOX_MAX 300
#define GEARBOX_MARGIN 100

// Constant servo positions
const uint16_t servo_home[6] = {374,370,0,0,0,0};
const uint16_t servo_active[6] = {206,209,0,0,0,0};

// Constant lever positions
const int16_t lever_x[6] = {0,0,0,0,0,0};
const int16_t lever_y[6] = {0,0,0,0,0,0};

// Constant lever pins
const uint8_t lever_pin[6] = {8,9,10,11,12,13};

// Constant quadrature pins
const uint8_t quadrature_pin[6] = {0,2,4,6,24,26};

#define FOOT 443.4
#define LEVER_DIST_SQ ((FOOT*1.5)*(FOOT*1.5))

// --------------------

uint8_t round_running = 0;

// When the round started
uint32_t round_start_ms = 0;

// Keep track of which robot is which
uint8_t robot_ids[2] = {170, 170};
uint16_t scores[2] = {0,0};

// Territory data
int16_t value[6] = {0,0,0,0,0,0};
uint8_t owner[6] = {-1,-1,-1,-1,-1,-1};



float dist_sq(float x1, float y1, float x2, float y2) {
    return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


void uround_start(){
    int i;
    copy_objects();

    printf("Robots: %d and %d\n", objects[0].id, objects[1].id);

    // determine which robot is which
    if (objects[0].id == 170 || objects[1].id == 170) {
        printf("FAIL! Missing one or more robot!\n");
        return;
    }

    if (objects[0].x < 0 && objects[1].x > 0) {
        robot_ids[0] = objects[0].id;
        robot_ids[1] = objects[1].id;
    } else if (objects[0].x > 0 && objects[1].x < 0) {
        robot_ids[0] = objects[1].id;
        robot_ids[1] = objects[0].id;
    } else {
        printf("FAIL! Robots not on distinct halves! x1:%d, x2:%d\n", objects[0].x, objects[1].x);
    }

    scores[0] = 0;
    scores[1] = 0;

    for (i=0; i<6; i++) {
        owner[i] = -1;
        value[i] = 0;
    }

    round_start_ms = get_time();

    round_running = 1;
}

void uround_end(){
    round_running = 0;
}

int run_dispensers() {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        servo_set_pos(i, servo_home[i]);
    }


    bool last_lever = false;
    while(1) {

        // No dispensers allowed during first 10 seconds
        if (round_running == 0 || get_time() - round_start_ms < 10000) {
            pause(10);
            yield();
            continue;
        }


        // XXX: only handles one lever

        bool cur_lever = (analog_read(8) < 500);

        if (cur_lever && !last_lever) {
            for (i = 0; i < 6; i++) {
                if (owner[i] == 0) {
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


int run_gearboxes() {

    digital_write(6, 0);
    digital_write(7, 0);

    while(1) {
        // No capturing allowed during first 10 seconds
        if (round_running == 0 || get_time() - round_start_ms < 10000) {
            pause(10);
            yield();
            continue;
        }
        
        for (int i = 1; i < 2; i++) {
            // Add the quadrature delta
            value[i] += quadrature_read(quadrature_pin[i]);

            // bound the value
            if (value[i] > GEARBOX_MAX) {
                value[i] = GEARBOX_MAX;
            } else if (value[i] < -GEARBOX_MAX) {
                value[i] = -GEARBOX_MAX;
            }

            quadrature_reset(quadrature_pin[i]);

            if (value[i] > GEARBOX_MARGIN) {
                owner[i] = 0;
            } else if (value[i] < -GEARBOX_MARGIN) {
                owner[i] = 1;
            }
        }
        
        printf("Value: %d\n", value[0]);
        if (owner[1] == 0) {
            digital_write(6, 1);
            digital_write(7, 0);
        } else if (owner[1] == 1) {
            digital_write(6, 0);
            digital_write(7, 1);
        }


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
