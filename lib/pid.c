/*
 * The MIT License
 *
 * Copyright (c) 2007 MIT 6.270 Robotics Competition
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/**
 * lib/pid.c
 *
 * PID controller abstration library.
 *
 * The general idea is that users create an abstract PID controller that will
 * read values from a user-specified function and set an output value to another
 * user-specified function.
 *
 * Written by: Greg Belote [gbelote@mit.edu]
 */

#include <pid.h>
#include <thread.h>
#include <math.h>
#include <uart.h>

void
init_pid (struct pid_controller *pid, float kp, float ki, float kd,
		float (*input) (), void (*output) (float value)) {

	if (pid == 0)
		panic ("init_pid: pid null");

	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;

	pid->input = input;
	pid->output = output;

	pid->sum = 0;
	pid->has_past = 0;
	pid->enabled = 0;
}

float
update_pid (struct pid_controller *pid) {
	if (!pid->enabled) {
		panic ("update_pid: pid disabled");
	}

	if (!pid->input)
		panic ("update_pid: no input");
	if (!pid->output)
		panic ("update_pid: no output");

	float current_val = pid->input();

	uart_printf("current value %d.%03u\n",
			(int) current_val, 
			(int) floor(current_val*100-floor(current_val)*100));

	float error = pid->goal - current_val;

	uart_printf("error %d.%03u\n",
			(int) error, 
			(int) floor(error*100-floor(error)*100));

	// proportional feedback
	float result = error * pid->kp;

	float partial_sum = 0;
	if (pid->has_past) {
		float dt = 0.001 * (get_time() - pid->last_time);

		// integral feedback
		partial_sum = pid->sum + dt * error;
		result += pid->ki * partial_sum;

		// differential feedback
		if (dt != 0)
			result += pid->kd * (current_val - pid->last_val) / dt;
	}

	pid->has_past = 1;
	pid->sum = partial_sum;
	pid->last_val = current_val;
	pid->last_time = get_time();

	pid->output (result);

	return result;
}


uint8_t
dispatch_pid (struct pid_controller *pid, float tolerance, uint8_t (*stop) ()) {
	float result;
	do {
		// break if stopped
		//if (stop && stop())
		//	return 0; // return failure

		result = update_pid (pid);
	//} while (fabs (pid->goal - result) <= tolerance);
	} while (1);

	return 1; // return success
}

