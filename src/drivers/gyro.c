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
#include <stdlib.h>

double _lsb_ms_per_deg = 0;
volatile double _theta = 0;
uint8_t _port = 11;
volatile uint8_t _gyro_enabled = 0;
double _offset = 0;

uint16_t _num_samples = 29;
uint16_t *_samples;
volatile uint16_t _sum = 0;
volatile uint16_t _index = 0;
volatile uint8_t _sample_inited = 0;
volatile uint16_t _update_counter = 0;

// NOTE: happens during a ISR so interrupts are cleared
void
gyro_new_sample () {
	_sum -= _samples[_index];
	_samples [_index] = analog_read (_port);
	_sum += _samples[_index];

	_index++;

	if (_index >= _num_samples) {
		_sample_inited = 1;
		//_index = (_index % _num_samples);
		_index = 0;
	}

	_update_counter++;
}


double
gyro_read () {
	// use a lockless scheme for concurrent modification detection
	uint16_t counter;
	double result;

	do {
		counter = _update_counter;
		result = (double) _sum / (double) _num_samples;
	} while (counter != _update_counter);


/*
	do {
		// look at the counter before we start
		counter = _update_counter;
		sum = 0;

		for (uint16_t i = 0; i < _num_samples; i++) {
			//sum += _samples [(_index + i) % _num_samples];
			sum += _samples [i];
		}

	// if the counter has changed, start again
	} while (counter != _update_counter);
*/

	return result;
}

void
gyro_init(uint8_t port, double lsb_ms_per_deg, uint16_t samples) {

	uart_printf_P (PSTR("gyro_init (%u,%.2f,%u)\n"),
			port, lsb_ms_per_deg, samples);

	// record sample list size
	_num_samples = samples;

	// allocate sample list
	_samples = (uint16_t *) malloc (_num_samples * sizeof(uint16_t));
	for (uint16_t i = 0; i < _num_samples; i++)
		_samples[i] = 0;

	// record port of the gyro
	_port = port;

	uart_printf_P (PSTR("enabling isr\n"));

	// enable gyro and start reading measurments into sample list
	_gyro_enabled = 1;

	uart_printf_P (PSTR("waiting for sampling to finish\n"));

	// wait until list has filled
	while (!_sample_inited) {
		yield();
	}

	uart_printf_P (PSTR("init complete. grabbing stable value\n"));

	// assume robot has been sitting still and take current reading as offset
	for (uint8_t i = 0; i < 10; i++) {
		_offset += gyro_read ();
		pause (100);
	}

	_offset /= 10;

	uart_printf_P (PSTR("offset is %.2f\n"), _offset);

	// set angle to zero
	_theta = 0;

	// record LSB*ms/deg scale factor
	_lsb_ms_per_deg = lsb_ms_per_deg;

	uart_printf_P (PSTR("gyro_init complete\n"));
}

void
gyro_update() {
	// sample and add to queue
	gyro_new_sample ();

	// if we haven't finished initializing the sample queue, stop
	if (!_sample_inited)
		return;

	// assume a constant time interval between measurements---1ms
	// update theta
	_theta -= gyro_read () - _offset;
}

uint8_t
gyro_enabled (void) {
	return _gyro_enabled;
}

double
gyro_get_degrees (void) {
	uint8_t sreg = SREG & SREG_IF;
	cli ();
	double result = _theta / _lsb_ms_per_deg;

	SREG |= sreg;
	return result;
}

