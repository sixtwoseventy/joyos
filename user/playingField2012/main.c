#include <joyos.h>

extern volatile uint8_t robot_id;

int usetup() {
    robot_id = 0;
    return 0;
}

#define MAXINT 4294967295

#define GEARBOX_MAX 800
#define GEARBOX_MARGIN 200

#define LEVER_DEBOUNCE_MS 100
#define SERVO_MOVE_MS 300

// Constant servo positions
const uint16_t servo_home[6] = {370,370,0,0,0,0};
const uint16_t servo_active[6] = {206,209,0,0,0,0};

// Constant lever positions
const int16_t lever_x[6] = {1791,1280,-512,-1791,-1280,  512};
const int16_t lever_y[6] = {-443,1330,1773,  443,-1330,-1773};

// Constant lever pins
const uint8_t lever_pin[6] = {8,9,10,11,12,13};

// Constant quadrature pins
const uint8_t quadrature_pin[6] = {0,2,4,6,24,26};

#define FOOT 443.4
#define LEVER_DIST_SQ ((FOOT*1.5)*(FOOT*1.5))

#define EXPLORATION_MS 10000

#define SCORE_CAPTURE 100
#define SCORE_MINE 40
#define SCORE_DUMP 40
#define SCORE_EXPLORE 10
#define SCORE_EXPLORE_EARLY 10

// --------------------

uint8_t round_running = 0;

// When the round started
uint32_t round_start_ms = 0;

// Keep track of which robot is which
uint8_t robot_ids[2] = {170, 170};
uint16_t scores[2] = {0,0};

// Territory data
int16_t value[6] = {0,0,0,0,0,0};
int8_t owner[6] = {-1,-1,-1,-1,-1,-1};

uint32_t servo_home_time[6] = {0,0,0,0,0,0};

uint32_t lever_debounce_time[6] = {0,0,0,0,0,0};
uint32_t lever_reset_time[6] = {0,0,0,0,0,0};
uint8_t remaining_balls[6] = {0,0,0,0,0,0};


float dist_sq(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    float fx1 = (float)x1;
    float fy1 = (float)y1;
    float fx2 = (float)x2;
    float fy2 = (float)y2;
    return ((fx1-fx2)*(fx1-fx2) + (fy1-fy2)*(fy1-fy2));
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
        remaining_balls[i] = 10;
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


    uint8_t last_lever[6] = {1,1,1,1,1,1};
    while(1) {
        copy_objects();

        // maps robot id to objects array index
        uint8_t object_num[2];
        if (objects[0].id == robot_ids[0]) {
            object_num[0] = 0;
        } else if (objects[1].id == robot_ids[0]) {
            object_num[0] = 1;
        }

        if (objects[0].id == robot_ids[1]) {
            object_num[1] = 0;
        } else if (objects[1].id == robot_ids[1]) {
            object_num[1] = 1;
        }

        for (i = 0; i < 6; i++) {
            if (get_time() > servo_home_time[i]) {
                servo_set_pos(i, servo_home[i]);
            }
        }

        // No dispensers allowed during first 10 seconds
        if (round_running == 0 || get_time() - round_start_ms < EXPLORATION_MS) {
            pause(10);
            yield();
            continue;
        }


        for (i = 0; i < 6; i++) {
            uint8_t cur_lever = (analog_read(lever_pin[i]) < 500);

            if (cur_lever && !last_lever[i]) {
                lever_debounce_time[i] = get_time() + LEVER_DEBOUNCE_MS;
            }

            if (cur_lever && get_time() > lever_debounce_time[i] && get_time() > lever_reset_time[i]) {
                if (owner[i] != -1 && remaining_balls[i] > 0) { 
                    int16_t owner_x = objects[object_num[owner[i]]].x;
                    int16_t owner_y = objects[object_num[owner[i]]].y;
                    float owner_sq_dist = dist_sq(owner_x, owner_y, lever_x[i], lever_y[i]);

                    //printf("Pull. owner at (%d,%d), lever at (%d,%d).  owner dist sq = %.2f\n", owner_x, owner_y, lever_x[i], lever_y[i], owner_sq_dist);

                    if (owner_sq_dist < LEVER_DIST_SQ) {
                        servo_set_pos(i, servo_active[i]);
                        servo_home_time[i] = get_time() + SERVO_MOVE_MS;
                        lever_reset_time[i] = servo_home_time[i] + SERVO_MOVE_MS; // lever can be used again 300ms after setting the servo home
                        
                        lever_debounce_time[i] = MAXINT;

                        scores[owner[i]] += SCORE_MINE;

                        remaining_balls[i] -= 1;
                    }
                }
            }

            last_lever[i] = cur_lever;
        }

    }


    return 0;
}


void print_data() {
    /* Format:
       DATA:
       [robot id 0],
       [robot id 1];
       [score 0],
       [score 1];
       [ter 0 owner],
       [ter 1 owner],
       [ter 2 owner],
       [ter 3 owner],
       [ter 4 owner],
       [ter 5 owner],;
       [ter 0 balls left],
       [ter 1 balls left],
       [ter 2 balls left],
       [ter 3 balls left],
       [ter 4 balls left],
       [ter 5 balls left],;

    */
    printf("DATA:%u,%u;%u,%u;", robot_ids[0], robot_ids[1], scores[0], scores[1]);
    for (int i = 0; i < 6; i++) {
        printf("%d,", owner[i]);
    }
    printf(";");
    for (int i = 0; i < 6; i++) {
        printf("%u,", remaining_balls[i]);
    }
    printf("\n");
}


int run_gearboxes() {
    while(1) {
        // No capturing allowed during first 10 seconds
        if (round_running == 0 || get_time() - round_start_ms < EXPLORATION_MS) {
            pause(10);
            yield();
            continue;
        }
        
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

            if (value[i] > GEARBOX_MARGIN) {
                if (owner[i] != 0) {
                    scores[0] += SCORE_CAPTURE;
                }
                owner[i] = 0;
            } else if (value[i] < -GEARBOX_MARGIN) {
                if (owner[i] != 1) {
                    scores[1] += SCORE_CAPTURE;
                }
                owner[i] = 1;
            }
        }

        print_data();

        pause(10);
        yield();
    }
    return 0;
}

int umain() {
    uround_start();
    create_thread(&run_dispensers, STACK_DEFAULT, 0, "dispenser_thread");
    run_gearboxes();
    return 0;
}
