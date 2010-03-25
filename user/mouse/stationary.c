#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#include <joyos.h>

void initRun() {
    return;
}
void activateRun() {
    motor_set_vel(RIGHT_MOTOR,0);
    motor_set_vel(LEFT_MOTOR,0);
}
void step() {
    return;
}
