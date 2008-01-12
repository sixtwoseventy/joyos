#include <lib/motion.h>
#include <encoder.h>

// limit a value
#define limit(v, min, max) ((v)>(max)) ? (max) : ((v)<(min)) ? (min) : (v)

void 
motion_init(MotionController *motion, MotorGroup motor, uint8_t encoder_port, 
		float kp, float ki, float kd) {
	motion->position = 0;
	motion->encoder = encoder_port;
	motion->encoder_old_pos = encoder_read(encoder_port);
	motion->output = motor;
	init_pid(&(motion->pid), kp, ki, kd, NULL, NULL);
	motion->pid.enabled = true;
}

void 
motion_set_goal(MotionController *motion, int32_t goal) {
	motion->pid.goal = goal;
}

void 
motion_update(MotionController *motion) {
	float drive;
	// update our actual position with encoder reading.
	// FIXME this will probably explode when the encoder rolls over
	int16_t delta	= encoder_read(motion->encoder) - motion->encoder_old_pos;
	motion->position += delta;
  // run the PID to calculate our drive value
	drive = update_pid_input(&(motion->pid), motion->position);
	// apply the (limited) drive value to the motors
	motor_group_set_vel(motion->output, limit(drive, -255, 255));
}

bool 
motion_goal_reached(MotionController *motion) {
	return motion->position == ((int32_t)(motion->pid.goal));
}
