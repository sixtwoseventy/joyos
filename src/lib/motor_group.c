#include <lib/motor_group.h>
#include <motor.h>

MotorGroup 
motor_group_new(bool m0, bool m1, bool m2, bool m3, bool m4, bool m5) {
	uint8_t v = (m0 ?  1:0) |
							(m1 ?  2:0) |
							(m2 ?  4:0) |
							(m3 ?  8:0) |
							(m4 ? 16:0) |
							(m5 ? 32:0);
	return v;
}

void 
motor_group_set_vel(MotorGroup group, int16_t value) {
	for (uint8_t i=0; i<6; i++) {
		if (group & (1<<i))
			motor_set_vel(i, value);
	}
}
