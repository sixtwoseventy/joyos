#include <joyos.h>
#include <math.h>

extern volatile uint8_t robot_id;


void print_data();

#define MAXINT 4294967295


#define LEVER_DEBOUNCE_MS 100
#define LEVER_ANALOG_THRESHOLD 50
#define SERVO_MOVE_MS 800

#define BOARD_SIDE 1

/* mapping levers to servos
mapping levers to orange/blue:
((lever_number)%4)/2
mapping levers to servos
if(lever_number % 2 ==0): servo = ((lever_number / 2) * 3) % 6
else: servo1 = ((lever_number / 2) * 3 + 1) % 6, servo2 = ((lever_number / 2) * 3 + 2) % 6

*/

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


// NOTE: servo arrays are 12 long--both boards will "see" all servos, even ones they aren't connected to.
// Some math (presence or lack of a mod 6 operation) is used to keep the indices straight.

// Constant servo positions
const uint16_t servo_home[12] = {511,0,511,511,0,474,438,0,497,511,0,496};


const uint16_t servo_simple_active[12] = {0,-1,71,0,-1,0,0,-1,0,0,-1,0};
const uint16_t servo_low_active[12][4] = {{-1,-1,-1,-1},
			   		{93,222,329,455},
			   		{-1,-1,-1,-1},
			   		{-1,-1,-1,-1},
			   		{159,292,401,497},
			   		{-1,-1,-1,-1},
			   		{-1,-1,-1,-1},
			   		{107,228,317,472},
			   		{-1,-1,-1,-1},
			   		{-1,-1,-1,-1},
			   		{188,287,403,511},
			   		{-1,-1,-1,-1},};

// Constant lever positions
const int16_t lever_x[8] = {2047,1498,2047,1367,-1443,-2048,-1316,-2048};
const int16_t lever_y[8] = {1322,2047,-1512,-2048,-2048,-1311,2047,1423};

// Constant lever pins
const uint8_t lever_pin[8] = {8,9,10,11,12,13,14,15};



#define FOOT 443.4
#define LEVER_DIST_SQ ((FOOT*1.5)*(FOOT*1.5))



#define RATE_LIMIT_MS 30000
#define RATE_LIMIT_RINGS 4

// --------------------

uint8_t round_running = 0;

// When the round started
uint32_t round_start_ms = 0;

// Keep track of which robot is which
uint8_t robot_ids[2] = {170, 170};
uint16_t last_score_out[8] = {0,0,0,0,0,0,0,0};


uint32_t servo_home_time[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

uint32_t lever_debounce_time[8] = {0,0,0,0,0,0,0,0};
uint32_t lever_reset_time[8] = {0,0,0,0,0,0,0,0};
uint8_t available_rings[8] = {4,4,4,4,4,4,4,4};

uint32_t rate_limit_start_time[8] = {0,0,0,0,0,0,0,0};


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

    for (int i = 0+6*BOARD_SIDE; i < 6+6*BOARD_SIDE; i++) {
        test_servo(i);
    }

    printf("Test levers now.  Press stop to quit test.\n");
    while (!stop_press() && !round_running) {
        for (int i = 0; i < 8; i++) {
	    uint8_t a_read = analog_read(lever_pin[i]);
            uint8_t lever = a_read < LEVER_ANALOG_THRESHOLD; //anog pins go from 8 to something
	    printf("%d(%2d, %3u) ", lever, i, a_read);
	    
            if (lever) {
                if (i % 2 == 0) {
                    test_servo(i / 2 * 3);
                } else {
                    test_servo(i / 2 * 3 + 1);
                    test_servo(i / 2 * 3 + 2);
                }
            } else {
	      //servo_set_pos((i/2*3%6, servo_home[i]);
            }
        }
	printf("\n");
        pause(200);
    }

    return 0;
}

void test_servo(int i) {
    servo_set_pos(i%6, servo_home[i]);
    pause(SERVO_MOVE_MS);
    servo_set_pos(i%6, servo_home[i] - 20);
    pause(SERVO_MOVE_MS);
    servo_set_pos(i%6, servo_home[i]);
    pause(SERVO_MOVE_MS + 500);
}
  

void reset_round() {
    robot_ids[0] = 0;
    robot_ids[1] = 0;
    
    for (int i = 0; i < 8; i++){
        last_score_out[i] = 1 - digital_read(i);
    }
    
    
    // robots are already in a territory at the beginning, they shouldn't get exploration points
    
    for (int i = 0; i < 8; i++) {
        available_rings[i] = RATE_LIMIT_RINGS;
        rate_limit_start_time[i] = MAXINT;
    }
}

void uround_start(){
    copy_objects();
    
    printf("Robots: %d and %d\n", game.coords[0].id, game.coords[1].id);
    reset_round();

    // determine which robot is which
    if (game.coords[0].id != 170 && game.coords[1].id != 170) {
        if ( (game.coords[0].y < 0 && game.coords[1].y < 0) ||
             (game.coords[0].y > 0 && game.coords[1].y > 0) ) {
            printf("FAIL! Robots not on distinct halves! y1:%d, y2:%d\n", game.coords[0].y, game.coords[1].y);
            pause(300);
        }
    }

    if (game.coords[0].id != 170) {
        if (game.coords[0].y < 0) {
            robot_ids[0] = game.coords[0].id;
        } else {
            robot_ids[1] = game.coords[0].id;
        }
    }

    if (game.coords[1].id != 170) {
        if (game.coords[1].y < 0) {
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

void dispense_ring (int lever){
	if (lever % 2 == 0){
		int serv_num = lever/2*3;
		servo_set_pos(serv_num % 6, servo_simple_active[serv_num]);
		servo_home_time[serv_num] = get_time() + SERVO_MOVE_MS;
		printf("home time %u", servo_home_time[serv_num]);
		printf("now time %u", get_time());
	}else{
		int serv_num_f = lever/2*3 + 1;
		int serv_num_b = lever/2*3 + 2;
		servo_set_pos(serv_num_f % 6, servo_low_active[serv_num_f][available_rings[lever] % 4]);
		servo_home_time[serv_num_f] = get_time() + SERVO_MOVE_MS;
		
		if (available_rings[lever] % 4 == 1){
			//pause(SERVO_MOVE_MS); //a better way to do this?
			servo_set_pos(serv_num_b % 6, servo_simple_active[serv_num_b]);
			servo_home_time[serv_num_b] = get_time() + SERVO_MOVE_MS;
		}
	}


}

int run_dispensers() {
    uint8_t i;
    for (i = 0+6*BOARD_SIDE; i < 6+6*BOARD_SIDE; i++) {
        servo_set_pos(i%6, servo_home[i]);
    }


    uint8_t last_lever[8] = {1,1,1,1,1,1,1,1};
    while(1) {
        //print_data(); Since boards not connected to anything over serial, no reason to print the DATA
        pause(10);

        copy_objects();
//printf("Robot %d: (%d, %d)\n", 0, game.coords[0].x, game.coords[0].y);
//printf("Robot %d: (%d, %d)\n", 1, game.coords[1].x, game.coords[1].y);
//printf("Size of game data: %d\n", sizeof(game));
	
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


        // controls resetting servos--servo by servo basis
        for (i = 0+6*BOARD_SIDE; i < 6+6*BOARD_SIDE; i++) {
            if (get_time() > servo_home_time[i]) {
                servo_set_pos(i%6, servo_home[i]);
                servo_home_time[i] = MAXINT; // added so that servo_set_pos() isn't called a bunch of times needlessly. may break when other things change.
            }
        }
        // controls resetting rate limit--lever by lever basis
        for (i = 0; i < 8; i++) {
            // if the rate limit timer expired, make more balls available and clear the timer
            if (get_time() > rate_limit_start_time[i] + RATE_LIMIT_MS) {
printf("Lever: %d;  Get time: %d;  Rate Limit Start Time: %d\n", i, get_time(), rate_limit_start_time[i]);
                available_rings[i] = RATE_LIMIT_RINGS;
printf("Available Rings: %d at lever %d", available_rings[i], i);
                rate_limit_start_time[i] = MAXINT;
            }
        }


uint8_t changed = 0;
        for (i = 0; i < 8; i++) {
            uint8_t cur_lever = (analog_read(lever_pin[i]) < LEVER_ANALOG_THRESHOLD);

            if (cur_lever && !last_lever[i]) {
                lever_debounce_time[i] = get_time() + LEVER_DEBOUNCE_MS;
            }

            if (cur_lever && get_time() > lever_debounce_time[i] && get_time() > lever_reset_time[i]) {
                if (available_rings[i] > 0) {
                    int owner = object_num[1 - (i % 4) / 2];
                    int owner_x = game.coords[owner].x;
                    int owner_y = game.coords[owner].y;
                    float owner_sq_dist = dist_sq(owner_x, owner_y, lever_x[i], lever_y[i]);

printf("Owner:%d, lever:%d at (%d,%d), lever at (%d,%d).  owner dist sq = %.2f\n", game.coords[owner].id, i, owner_x, owner_y, lever_x[i], lever_y[i], owner_sq_dist);

                    if (owner_sq_dist < LEVER_DIST_SQ) {
                        dispense_ring(i);
                        lever_reset_time[i] = get_time() + 2*SERVO_MOVE_MS; // lever can be used again 300ms after setting the servo home
                        lever_debounce_time[i] = MAXINT;

                        last_score_out[i] = 1 - last_score_out[i];
                        digital_write(i, last_score_out[i]);
                        printf("writing %d to %d\n", last_score_out[i], i);
changed = 1;

                        available_rings[i] -= 1;
printf("Available Rings at %d: %d\n", i, available_rings[i]);

                        if (available_rings[i] == 0) {
                            // start the rate limit
                            rate_limit_start_time[i] = get_time();
                        }
                    }
                }
            }
            last_lever[i] = cur_lever;
        }
if (changed) {
            for (i = 0; i < 8; i++) {
                printf("%d:%d; ", i, last_score_out[i]);
            }
            printf("\n");
}
        
    }


    return 0;
}
/** DRAFT see dispense_ring
void lever_pulled(int i) {
    if (i % 2 == 0) {
        int serv_num = i / 2 * 3;
	servo_set_pos(serv_num % 6, servo_active[serv_num]);
	servo_home_time[serv_num] = get_time() + SERVO_MOVE_MS;
    } else {
        int serv_num = i / 2 * 3 + 1;
	servo_set_pos(serv_num % 6, servo_active[serv_num]);
	servo_home_time[serv_num] = get_time() + SERVO_MOVE_MS;

	serv_num = i / 2 * 3 + 2;
	servo_set_pos(serv_num % 6, servo_active[serv_num]);
	servo_home_time[serv_num] = get_time() + SERVO_MOVE_MS;
    }
}
**/


int umain() {
    uround_start();
    create_thread(&run_dispensers, STACK_DEFAULT, 0, "dispenser_thread");
    return 0;
}
