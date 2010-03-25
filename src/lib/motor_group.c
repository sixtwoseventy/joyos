#include <lib/motor_group.h>
#include <motor.h>

MotorGroup motor_group_new(bool m0, bool m1, bool m2, bool m3, bool m4, bool m5) {
    uint8_t v = (m0 ?  1:0) | (m1 ?  2:0) |
        (m2 ?  4:0) | (m3 ?  8:0) |
        (m4 ? 16:0) | (m5 ? 32:0);
    return v;
}

void motor_group_set_vel(MotorGroup group, int16_t vel) {
    for (uint8_t i=0; i<6; i++) {
        if (motor_group_has(group, i))
            motor_set_vel(i, vel);
    }
}

void motor_group_brake(MotorGroup group) {
    for (uint8_t i=0; i<6; i++) {
        if (motor_group_has(group, i))
            motor_brake(i);
    }
}
