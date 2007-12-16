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

#include <board.h>
#include <gyro.h>
#include <hal/io.h>
#include <kern/global.h>
#include <kern/lock.h>
#include <kern/thread.h>

uint32_t _lsb_ms_per_deg = 0;
int32_t _theta = 0;
uint8_t _port = 11;
uint8_t _gyro_enabled = 0;
uint32_t _offset = 0;
uint32_t _last_time = 0;

void
gyro_init(uint8_t port, uint16_t lsb_ms_per_deg, uint32_t samples) {
	uint32_t sum = 0.0;

	//dump_threadstates();
	for (uint32_t i = 0; i < samples; i++) {
		sum += analog_read (port);
	}

	_offset = sum / samples;
	_theta = 0;
	_lsb_ms_per_deg = lsb_ms_per_deg;
	_port = port;

	extern uint32_t global_time;
	_last_time = global_time;

	_gyro_enabled = 1;
}

void
gyro_update() {
	extern uint32_t global_time;

	uint32_t current_time = global_time;
	uint32_t ms_dt = current_time - _last_time;
	_last_time = current_time;

	uint16_t sum = 0;
	const uint8_t samples = 3;
	for (uint8_t i = 0; i < samples; i++) {
		sum += analog_read (_port);
	}
	//uint16_t analog_value = sum / 10;
	uint16_t analog_value = sum / samples;

	//uart_printf("analog_value: %d\n", analog_value);

	_theta -= (analog_value - _offset) * ms_dt;
}

uint8_t
gyro_enabled (void) {
	return _gyro_enabled;
}

float
gyro_get_degrees (void) {
	uint8_t sreg = SREG & SREG_IF;
	cli ();
	float result = ((float) _theta) / _lsb_ms_per_deg;

	SREG |= sreg;
	return result;
}

