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

#ifndef _MAX1230_H_
#define _MAX1230_H_

#define MAX1230_READY		0
#define MAX1230_SPI_BUSY	-1

typedef enum {
	MAX1230_CH0 		= (0x0 << 3),
	MAX1230_CH1 		= (0x1 << 3),
	MAX1230_CH2 		= (0x2 << 3),
	MAX1230_CH3 		= (0x3 << 3),
	MAX1230_CH4 		= (0x4 << 3),
	MAX1230_CH5 		= (0x5 << 3),
	MAX1230_CH6 		= (0x6 << 3),
	MAX1230_CH7 		= (0x7 << 3),
	MAX1230_CH8 		= (0x8 << 3),
	MAX1230_CH9 		= (0x9 << 3),
	MAX1230_CH10		= (0xA << 3),
	MAX1230_CH11		= (0xB << 3),
	MAX1230_CH12		= (0xC << 3),
	MAX1230_CH13		= (0xD << 3),
	MAX1230_CH14		= (0xE << 3),
	MAX1230_CH15		= (0xF << 3),
	MAX1230_TEMP		= 0x01
} Max1230Channel;

typedef enum {
	MAX1230_SCAN_0_N		= (0x0 << 1),
	MAX1230_SCAN_N_MAX	= (0x1 << 1),
	MAX1230_SCAN_N_AVG	= (0x2 << 1),
	MAX1230_SCAN_NONE		= (0x3 << 1),
} Max1230Scan;

uint8_t max1230Init();
uint8_t max1230SingleReading(Max1230Channel chan, uint16_t *val);

#endif
