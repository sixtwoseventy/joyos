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

#ifndef SIMULATE
#include <joyos.h>

float _lsb_us_per_deg = 0;
volatile int64_t _theta = 0;
volatile int32_t _theta_fix = 0;
uint8_t _port = 11;
volatile uint8_t _gyro_enabled = 0;
uint16_t _offset = 0, _offset_fix = 0;
uint16_t _fix_divisor;
struct lock gyro_lock;
#else
#include <joyos.h>
#include <stdio.h>
#include <socket.h>
#endif

void gyro_init(uint8_t port, float lsb_us_per_deg, uint32_t time_ms) {
	#ifndef SIMULATE
    uint32_t samples;
    uint32_t sum;
    uint32_t start_time_ms;

    _port = port;
    _lsb_us_per_deg = lsb_us_per_deg;

    // average some samples for offset
    sum = 0.0;
    samples = 0;
    start_time_ms = get_time ();
    while ( (get_time() - start_time_ms) < time_ms ) {
        sum += analog_read (_port);
        samples ++;
    }

    _offset = (uint16_t)(sum / samples);
    _offset_fix = (sum % samples);
    _fix_divisor = samples;
    _theta = 0;

    init_lock (&gyro_lock, "gyro lock");
    create_thread (&gyro_update, STACK_DEFAULT, 0, "gyro");
	#endif
}

#ifndef SIMULATE
int gyro_update(void) {
    /* The units of theta are converter LSBs * ms
       1 deg/s = 5mV = 0.256 LSB for ADXRS300, 12.5mV = 0.64LSB for ADXRS150
       1 deg = 0.256LSB * 1000ms = 256 (stored in lsb_ms_per_deg)
       To convert theta to degrees, divide by lsb_ms_per_deg. */

    uint32_t delta_t_us, new_time_us, time_us;
    int32_t analog_value, fix_value;

    time_us = get_time_us();

    for(;;) {
        analog_value = (int32_t)analog_read(_port) - _offset;
        new_time_us = get_time_us();
        delta_t_us = (new_time_us - time_us);
        int32_t dtheta = analog_value * delta_t_us;

        fix_value = _offset_fix * delta_t_us;

        /* This does the Riemann sum; CCW gyro output polarity is negative
           when the gyro is visible from the top of the robot. */
        acquire (&gyro_lock);
        // accumulate fractional error
        _theta_fix += fix_value;
        // apply error correction
        // negative because _offset is subtracted
        // from dtheta
        dtheta -= _theta_fix / _fix_divisor;
        _theta_fix %= _fix_divisor;
        // integrate angle
        _theta -= dtheta;
        release (&gyro_lock);

        time_us = new_time_us;
        yield();
    }

    return 0;
}
#endif

float gyro_get_degrees (void) {
	#ifndef SIMULATE

    acquire (&gyro_lock);
    float result = _theta / _lsb_us_per_deg;
    release (&gyro_lock);

    return result;

	#else

	float in;

	acquire(&socket_lock);
	write(sockfd, "a\n", 2);
	read(sockfd, socket_buffer, SOCKET_BUF_SIZE);
	sscanf(socket_buffer, "%f", &in);
	release(&socket_lock);
  
	return in;

	#endif
}

#ifndef SIMULATE
void gyro_set_degrees (float deg) {
    acquire (&gyro_lock);
    _theta = deg * _lsb_us_per_deg;
    release (&gyro_lock);
}
#endif

