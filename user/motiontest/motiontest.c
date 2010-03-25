#include <board.h>
#include <kern/thread.h>
#include <lib/motion.h>
#include <lib/motor_group.h>

// setup geartrain
#define ENCODER_TO_WHEEL_RATIO 3
#define WHEEL_CIRCUMFERENCE    10.0
#define WHEEL_TRACK            20.0
#include <lib/geartrain.h>

// Encoder ports
#define ENCODER_LEFT  24
#define ENCODER_RIGHT 25

// PID values for motion controllers
#define KP 10
#define KI 0
#define KD 0

// Motion controllers and motor groups
MotionController mc_left, mc_right;
MotorGroup mg_left, mg_right;

int usetup (void) {
    return 0;
}

int umain (void) {
    float done = false;

    // set up motor groups (one motor per group)
    mg_left  = motor_group_new(1,0,0,0,0,0);
    mg_right = motor_group_new(0,1,0,0,0,0);
    // set up motion controllers
    motion_init(&mc_left,  mg_left,  ENCODER_LEFT,  KP, KI, KD);
    motion_init(&mc_right, mg_right, ENCODER_RIGHT, KP, KI, KD);
    // set target position (30cm forward)
    motion_set_goal(&mc_left,  CM_TO_TICKS(30));
    motion_set_goal(&mc_right, CM_TO_TICKS(30));

    while (!done) {
        // update motion controllers
        motion_update(&mc_left);
        motion_update(&mc_right);
        // quit once both controllers reach target
        done = motion_goal_reached(&mc_left) && motion_goal_reached(&mc_right);
    }
    // turn off all motors
    motor_group_set_vel(MOTOR_GROUP_ALL, 0);
    return 0;
}
