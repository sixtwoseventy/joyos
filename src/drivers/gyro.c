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

#include <joyos.h>

float _lsb_ms_per_deg = 0;
volatile float _theta = 0;
uint8_t _port = 11;
volatile uint8_t _gyro_enabled = 0;
float _offset = 0;
struct lock gyro_lock;

void
gyro_init(uint8_t port, float lsb_ms_per_deg, uint32_t time_ms) {
	uint32_t samples;
	float sum;
	uint32_t start_time_ms;

	_port = port;
	_lsb_ms_per_deg = lsb_ms_per_deg;

	// average some samples for offset
	sum = 0.0;
	samples = 0;
	start_time_ms = get_time ();
	while ( (get_time() - start_time_ms) < time_ms ) {
		sum += (float) analog_read (_port);
		samples ++;
	}

	_offset = sum / (float) samples;
	_theta = 0.0;

	init_lock (&gyro_lock, "gyro lock");
	create_thread (&gyro_update, STACK_DEFAULT, 0, "gyro");
}

int
gyro_update(void) {
	/* The units of theta are converter LSBs * ms
	   1 deg/s = 5mV = 0.256 LSB for ADXRS300, 12.5mV = 0.64LSB for ADXRS150
	   1 deg = 0.256LSB * 1000ms = 256 (stored in lsb_ms_per_deg)
	   To convert theta to degrees, divide by lsb_ms_per_deg. */

	uint32_t delta_t_ms, new_time_ms, analog_value, time_ms;

	time_ms = get_time();

	for(;;) {
		analog_value = analog_read(_port);
		new_time_ms = get_time();
		delta_t_ms = (new_time_ms - time_ms);
		float dTheta = ((float) analog_value - _offset) * (float) delta_t_ms;

		/* This does the Riemann sum; CCW gyro output polarity is negative
		   when the gyro is visible from the top of the robot. */
		acquire (&gyro_lock);
		_theta -= dTheta;
		release (&gyro_lock);

		time_ms = new_time_ms;
		yield();
	}

	return 0;
}

float
gyro_get_degrees (void) {
	acquire (&gyro_lock);
	float result = _theta / _lsb_ms_per_deg;
	release (&gyro_lock);

	return result;
}

void
gyro_set_degrees (float deg) {
	acquire (&gyro_lock);
	_theta = deg * _lsb_ms_per_deg;
	release (&gyro_lock);
}

