#include <joyos.h>
#include <math.h>

extern volatile uint8_t robot_id;


void print_data();

#define MAXINT 4294967295

#define GEARBOX_MAX 500
#define GEARBOX_MARGIN 200

#define LEVER_DEBOUNCE_MS 100
#define LEVER_ANALOG_THRESHOLD 500
#define SERVO_MOVE_MS 300


/*
A
373
208

B
230
83

C
368
212

D
337
186

E
372
210

F
482
324

0 - D
1 - E
2 - F
3 - A
4 - B
5 - C
*/


// Constant servo positions
const uint16_t servo_home[6] =   {337,372,482,365,230,368};
const uint16_t servo_active[6] = {186,210,324,208, 83,212};

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
#define SCORE_EXPLORE_EARLY 30

#define RATE_LIMIT_MS 60000
#define RATE_LIMIT_BALLS 5

// --------------------

uint8_t round_running = 0;

// When the round started
uint32_t round_start_ms = 0;

// Keep track of which robot is which
uint8_t robot_ids[2] = {170, 170};
uint16_t scores[2] = {0,0};
uint8_t visited_territories[2] = {0,0}; //bit indicators of exploration, terr 0 is LSB

// Territory data
int16_t value[6] = {0,0,0,0,0,0};
int8_t owner[6] = {-1,-1,-1,-1,-1,-1};

uint32_t servo_home_time[6] = {0,0,0,0,0,0};

uint32_t lever_debounce_time[6] = {0,0,0,0,0,0};
uint32_t lever_reset_time[6] = {0,0,0,0,0,0};
uint8_t available_balls[6] = {0,0,0,0,0,0};

uint32_t rate_limit_start_time[6] = {0,0,0,0,0,0};


float dist_sq(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    float fx1 = (float)x1;
    float fy1 = (float)y1;
    float fx2 = (float)x2;
    float fy2 = (float)y2;
    return ((fx1-fx2)*(fx1-fx2) + (fy1-fy2)*(fy1-fy2));
}

int usetup() {
    robot_id = 0;

    printf("Startup test, testing servos...\n");

    for (int i = 0; i < 6; i++) {
        servo_set_pos(i, servo_home[i]);
        pause(SERVO_MOVE_MS);
        servo_set_pos(i, servo_home[i] - 20);
        pause(SERVO_MOVE_MS);
        servo_set_pos(i, servo_home[i]);
        pause(500);
    }

    printf("Test gearboxes and levers now.  Press stop to quit test.\n");
    while (!stop_press() && !round_running) {
        for (int i = 0; i < 6; i++) {
            int16_t val = quadrature_read(quadrature_pin[i]);
            quadrature_reset(quadrature_pin[i]);

            uint8_t lever = analog_read(lever_pin[i]) < LEVER_ANALOG_THRESHOLD;
            if (val < -5 || val > 5 || lever) {
                servo_set_pos(i, servo_home[i] - 20);
            } else {
                servo_set_pos(i, servo_home[i]);
            }
        }
        pause(200);
    }

    return 0;
}

void reset_round() {
    robot_ids[0] = 0;
    robot_ids[1] = 0;

    scores[0] = 0;
    scores[1] = 0;

    // robots are already in a territory at the beginning, they shouldn't get exploration points
    visited_territories[0] = 1 << 3;
    visited_territories[1] = 1 << 0;

    for (int i=0; i<6; i++) {
        owner[i] = -1;
        value[i] = 0;
        available_balls[i] = RATE_LIMIT_BALLS;
        rate_limit_start_time[i] = 0;
    }
}

void uround_start(){
    copy_objects();

    printf("Robots: %d and %d\n", game.coords[0].id, game.coords[1].id);
    reset_round();

    // determine which robot is which
    if (game.coords[0].id != 170 && game.coords[1].id != 170) {
        if ( (game.coords[0].x < 0 && game.coords[1].x < 0) ||
             (game.coords[0].x > 0 && game.coords[1].x > 0) ) {
            printf("FAIL! Robots not on distinct halves! x1:%d, x2:%d\n", game.coords[0].x, game.coords[1].x);
            pause(300);
        }
    }

    if (game.coords[0].id != 170) {
        if (game.coords[0].x < 0) {
            robot_ids[0] = game.coords[0].id;
        } else {
            robot_ids[1] = game.coords[0].id;
        }
    }

    if (game.coords[1].id != 170) {
        if (game.coords[1].x < 0) {
            robot_ids[0] = game.coords[1].id;
        } else {
            robot_ids[1] = game.coords[1].id;
        }
    }


    round_start_ms = get_time();

    round_running = 1;
}

void uround_end(){
    round_running = 0;

    reset_round();
}

int run_dispensers() {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        servo_set_pos(i, servo_home[i]);
    }


    /*
    // Dispense all the things
    while(1) {
        for (i = 0; i < 6; i++) {
            servo_set_pos(i, servo_home[i]);
        }
        pause(300);
        for (i = 0; i < 6; i++) {
            servo_set_pos(i, servo_active[i]);
        }
        pause(300);
    }
    */


    uint8_t last_lever[6] = {1,1,1,1,1,1};
    while(1) {
        print_data();
        pause(10);

        copy_objects();

        // maps robot id to objects array index
        uint8_t object_num[2];
        if (game.coords[0].id == robot_ids[0]) {
            object_num[0] = 0;
        } else if (game.coords[1].id == robot_ids[0]) {
            object_num[0] = 1;
        }

        if (game.coords[0].id == robot_ids[1]) {
            object_num[1] = 0;
        } else if (game.coords[1].id == robot_ids[1]) {
            object_num[1] = 1;
        }

        
        // Check exploration
        if (round_running) {
            for (int i = 0; i < 2; i++) {
                if (dist_sq(0,0,game.coords[object_num[i]].x,game.coords[object_num[i]].y) > (2047.*2047.)) {
                    continue;
                }
                float rad = atan2(game.coords[object_num[i]].y, game.coords[object_num[i]].x);
                rad += M_PI / 6;  //rotate by 30 degrees so terr 0 starts at 0 theta
                rad += 2*M_PI;       //add 360 so it's positive
                rad = fmod(rad, 2*M_PI);

                int territory = (int)(rad / (M_PI/3));

                if ((visited_territories[i] & (1 << territory)) == 0) {
                    printf("Team %d visited territory %d! %f\n", i, territory, rad);
                    // newly visited territory!
                    if (get_time() - round_start_ms < EXPLORATION_MS) {
                        scores[i] += SCORE_EXPLORE_EARLY;
                    } else {
                        scores[i] += SCORE_EXPLORE;
                    }
                }

                visited_territories[i] |= (1 << territory);
            }
        }




        for (i = 0; i < 6; i++) {
            if (get_time() > servo_home_time[i]) {
                servo_set_pos(i, servo_home[i]);
            }

            // if the rate limit timer expired, make more balls available and clear the timer
            if (get_time() > rate_limit_start_time[i] + RATE_LIMIT_MS) {
                available_balls[i] = RATE_LIMIT_BALLS;
                rate_limit_start_time[i] = 0;
            }
        }

        // No dispensers allowed during first 10 seconds
        if (round_running == 0 || get_time() - round_start_ms < EXPLORATION_MS) {
            pause(10);
            yield();
            continue;
        }


        for (i = 0; i < 6; i++) {
            uint8_t cur_lever = (analog_read(lever_pin[i]) < LEVER_ANALOG_THRESHOLD);

            if (cur_lever && !last_lever[i]) {
                lever_debounce_time[i] = get_time() + LEVER_DEBOUNCE_MS;
            }

            if (cur_lever && get_time() > lever_debounce_time[i] && get_time() > lever_reset_time[i]) {
                if (owner[i] != -1 && available_balls[i] > 0) { 
                    int16_t owner_x = game.coords[object_num[owner[i]]].x;
                    int16_t owner_y = game.coords[object_num[owner[i]]].y;
                    float owner_sq_dist = dist_sq(owner_x, owner_y, lever_x[i], lever_y[i]);

                    //printf("Pull. owner at (%d,%d), lever at (%d,%d).  owner dist sq = %.2f\n", owner_x, owner_y, lever_x[i], lever_y[i], owner_sq_dist);

                    if (owner_sq_dist < LEVER_DIST_SQ) {
                        servo_set_pos(i, servo_active[i]);
                        servo_home_time[i] = get_time() + SERVO_MOVE_MS;
                        lever_reset_time[i] = servo_home_time[i] + SERVO_MOVE_MS; // lever can be used again 300ms after setting the servo home
                        
                        lever_debounce_time[i] = MAXINT;

                        scores[owner[i]] += SCORE_MINE;

                        available_balls[i] -= 1;

                        if (rate_limit_start_time[i] == 0) {
                            // start the rate limit
                            rate_limit_start_time[i] = get_time();
                        }
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
        if (owner[i] == -1) {
            printf("0,");
        } else {
            printf("%d,", robot_ids[owner[i]]);
        }
    }
    printf(";");
    for (int i = 0; i < 6; i++) {
        printf("%02u,", available_balls[i]);
    }
    printf(";");
    for (int i = 0; i < 6; i++) {
        uint32_t time = get_time();
        if (time - rate_limit_start_time[i] > RATE_LIMIT_MS || rate_limit_start_time[i]==0) {
            printf("0,");
        } else {
            uint16_t rl = (RATE_LIMIT_MS - (time - rate_limit_start_time[i]))/1000; 
            printf("%u,", rl);
        }
    }

    printf("\n");
}


int run_gearboxes() {
    while(1) {
        // No capturing allowed during first 10 seconds
        if (round_running == 0 || get_time() - round_start_ms < EXPLORATION_MS) {
            for (int i = 0; i < 6; i++) {
                quadrature_reset(quadrature_pin[i]);
            }
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


        //printf("%+3i %+3i %+3i %+3i %+3i %+3i\n", value[0], value[1], value[2], value[3], value[4], value[5]);
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
