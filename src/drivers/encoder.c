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

#include <config.h>
#include <fpga.h>
#include <encoder.h>
#include <kern/lock.h>

struct lock encoder_lock;

static uint16_t encoder_zero[4];

void
encoder_init (void) {
	init_lock (&encoder_lock, "encoder lock");
}

void 
encoder_reset(uint8_t encoder) {
	encoder_zero[encoder-24] = encoder_read(encoder);
}

uint16_t 
encoder_read(uint8_t encoder) {
	acquire (&encoder_lock);
	uint16_t hi,lo;
	uint16_t result;
	uint8_t ebase = FPGA_ENCODER_BASE + (encoder-24)*FPGA_ENCODER_SIZE;
	lo = fpga_read_byte(ebase+FPGA_ENCODER_LO);
	hi = fpga_read_byte(ebase+FPGA_ENCODER_HI);
	result = ((hi<<8) | lo) - encoder_zero[encoder-24];
	release (&encoder_lock);
	return result;
}
