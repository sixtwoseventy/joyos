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

#ifndef __INCLUDE_PID_H__
#define __INCLUDE_PID_H__

#include <global.h>


struct pid_controller {
	// constants
	float kp;
	float ki;
	float kd;

	// input and output
	float (*input) ();
	void (*output) (float value);

	// goal
	float goal;

	// feedback memory
	float sum;
	uint8_t has_past; // if false, last_val and last_time are undefined
	float last_val;
	uint32_t last_time;

	// state
	uint8_t enabled;
};

/** Initialize a PID controller. */
void init_pid (struct pid_controller *pid, 
		float kp, float ki, float kd, 
		float (*input) (), void (*output) (float value));

float update_pid (struct pid_controller *pid);

/**
 * Dispatch to a routine that will drive the PID system until the output
 * equals 'goal' within 'tolerance'. This routine blocks and only returns
 * when done or 'stop' is non-zero and returns non-zero.
 */
uint8_t dispatch_pid (
		struct pid_controller *pid, float tolerance, uint8_t (*stop) ());

#endif // __INCLUDE_PID_H__

