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

#include "config.h"
#include "fpga.h"
#include "mcp3008.h"

#include <stdlib.h>
#include <lock.h>

#define MOTOR_CTL_COAST		0
#define MOTOR_CTL_FWD			1
#define MOTOR_CTL_REV			2
#define MOTOR_CTL_BRAKE		3

#define MOTOR_MA_PER_LSB 	16.11

struct lock motor_lock;

void init_motor (void) {
	init_lock(&motor_lock, "motor lock");
}

void motorSetVel(uint8_t motor, int16_t vel) {
	acquire(&motor_lock);
	uint8_t mbase = FPGA_MOTOR_BASE + motor*FPGA_MOTOR_SIZE;
	if (vel>0)
		fpgaWriteByte(mbase+FPGA_MOTOR_CTL, MOTOR_CTL_FWD);
	else if (vel<0)
		fpgaWriteByte(mbase+FPGA_MOTOR_CTL, MOTOR_CTL_REV);
	else
		fpgaWriteByte(mbase+FPGA_MOTOR_CTL, MOTOR_CTL_COAST);

	fpgaWriteByte(mbase+FPGA_MOTOR_VEL,abs(vel));
	release(&motor_lock);
}

void motorBrake(uint8_t motor) {
	acquire(&motor_lock);
	uint8_t mbase = FPGA_MOTOR_BASE + motor*FPGA_MOTOR_SIZE;
	fpgaWriteByte(mbase+FPGA_MOTOR_CTL, MOTOR_CTL_BRAKE);
	release(&motor_lock);
}

uint16_t motorGetCurrent(uint8_t motor) {
	acquire(&motor_lock);
	uint8_t adcPortMap[6] = {4,5,2,3,0,1};
	uint16_t v;
	mcp3008_get_sample(MCP3008_CH0+adcPortMap[motor],&v);
	release(&motor_lock);
	return v;
}

uint16_t motorGetCurrentMA(uint8_t motor) {
	return motorGetCurrent(motor)*MOTOR_MA_PER_LSB;
}
