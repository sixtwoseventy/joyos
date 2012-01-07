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

#include <config.h>
#include <fpga.h>
#include <encoder.h>
#include <kern/lock.h>
#include <kern/global.h>

#else

#include <joyos.h>
#include <stdio.h>
#include <string.h>
#include <socket.h>

#endif

#ifndef SIMULATE

struct lock encoder_lock;

#endif

extern void _set_digital_pinmode(uint8_t port, uint8_t output);

static uint16_t encoder_zero[4];
static int16_t quadrature_zero[6];

void encoder_init (void) {

	#ifndef SIMULATE

    init_lock (&encoder_lock, "encoder lock");

	#else

	memset(encoder_zero, 0, 4);

	#endif

}

uint16_t encoder_read_raw(uint8_t encoder) {

	#ifndef SIMULATE

    acquire (&encoder_lock);
    uint16_t hi,lo;
    uint16_t result;
    uint8_t ebase = FPGA_ENCODER_BASE + (encoder-24)*FPGA_ENCODER_SIZE;
    lo = fpga_read_byte(ebase+FPGA_ENCODER_LO);
    hi = fpga_read_byte(ebase+FPGA_ENCODER_HI);
    result = ((hi<<8) | lo);
    release (&encoder_lock);
    return result;

	#else

	uint16_t in;
	char pbuf[6];

	sprintf(pbuf, "n %u\n", encoder);

	acquire(&socket_lock);
	write(sockfd, pbuf, strlen(pbuf));
	read(sockfd, socket_buffer, SOCKET_BUF_SIZE);
	sscanf(socket_buffer, "%u", &in);
	release(&socket_lock);
  
	return in;

	#endif

}


// based on a pin, find the fpga address offset for the quadrature count
// pins 0,1 -> 0
// pins 2,3 -> 1
// pins 4,5 -> 2
// pins 6,7 -> 3
// pins 24,25 -> 4
// pins 26,27 -> 5
static inline uint8_t _quadrature_offset(uint8_t pin) {
    if (pin < 8) {
        return pin / 2;
    } else if (pin >= 8 && pin < 24) { 
        panic("_quadrature_offset");
    } else if (pin >= 24 && pin <28) {
        return (pin / 2) - 8; // shift pin 24 down to quadrature index 4
    } else {
        panic("_quadrature_offset");
    }
}

int16_t quadrature_read_raw(uint8_t quadrature_index) {
    acquire (&encoder_lock);
    uint8_t hi,lo;
    int16_t result;

    uint8_t ebase = FPGA_QUADRATURE_BASE + quadrature_index*FPGA_QUADRATURE_SIZE;
    lo = fpga_read_byte(ebase+FPGA_QUADRATURE_LO);
    hi = fpga_read_byte(ebase+FPGA_QUADRATURE_HI);
    result = ((hi<<8) | lo);
    release (&encoder_lock);
    return result;
}

void encoder_reset(uint8_t encoder) {
    encoder_zero[encoder-24] = encoder_read_raw(encoder);
}

void quadrature_reset(uint8_t pin) {
    if (pin <= 7) {
        // digital pins need to be set as inputs for quadrature to work
        _set_digital_pinmode(pin, 0);
    }
    uint8_t offset = _quadrature_offset(pin);
    quadrature_zero[offset] = quadrature_read_raw(offset);
}

uint16_t encoder_read(uint8_t encoder) {
    return encoder_read_raw(encoder) - encoder_zero[encoder-24];
}

int16_t quadrature_read(uint8_t pin) {
    uint8_t offset = _quadrature_offset(pin);
    return quadrature_read_raw(offset) - quadrature_zero[offset];
}
