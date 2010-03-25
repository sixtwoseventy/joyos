#include <lib/motion.h>
#include <encoder.h>

// limit a value
#define limit(v, min, max) ((v)>(max)) ? (max) : ((v)<(min)) ? (min) : (v)

void motion_init(MotionController *motion, MotorGroup motor, uint8_t encoder_port, float kp, float ki, float kd) {
    motion->position = 0;
    motion->encoder = encoder_port;
    motion->encoder_old_pos = encoder_read(encoder_port);
    motion->output = motor;
    init_pid(&(motion->pid), kp, ki, kd, NULL, NULL);
    motion->pid.enabled = true;
}

void motion_set_goal(MotionController *motion, int32_t goal) {
    motion->pid.goal = goal;
}

// take 2 encoder readings and return an int32 of the delta
// handle rollover gracefully.
int32_t delta_roll(uint16_t new, uint16_t old) {
    uint16_t diff = new-old;
    //printf("n:%5d o:%5d diff: %5d | ",new,old,diff);
    if (diff>32768)
        return -65536+(int32_t)diff;
    return diff;
}

void motion_update(MotionController *motion) {
    float drive;
    // update our actual position with encoder reading.
    int32_t delta = delta_roll(encoder_read(motion->encoder),motion->encoder_old_pos);
    motion->position += delta;
    // run the PID to calculate our drive value
    drive = update_pid_input(&(motion->pid), motion->position);
    // apply the (limited) drive value to the motors
    motor_group_set_vel(motion->output, limit(drive, -255, 255));
}

bool motion_goal_reached(MotionController *motion) {
    return motion->position == ((int32_t)(motion->pid.goal));
}
