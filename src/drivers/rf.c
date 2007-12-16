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
#include <rf.h>
#include <nrf24l01.h>
#include <kern/thread.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//#define TIMER_PRESCALER (64)
//#define TIMER_1MS_EXPIRE ((uint8_t)(0xff-(F_CPU/TIMER_PRESCALER/1000)+1))

// Robot positions
static int16_t robot0_x;
static int16_t robot0_y;
static int16_t robot1_x;
static int16_t robot1_y;

// Competition round state 
static uint8_t roundState;

// This robot's team number
static uint8_t teamNumber;

void rf_update();
uint8_t rf_have_data();

void 
rf_init() {
	nrf_init_rx();
	roundState = STATE_STOPPED;
	teamNumber = 0;
}

void 
rf_set_team(uint8_t num) {
	teamNumber = num;
}

uint8_t 
rf_get_round_state() {
	return roundState;
}

uint8_t 
rf_have_data() {
	uint8_t s = nrf_read_reg(NRF_REG_STATUS);
	return (s&_BV(NRF_BIT_RX_DR));
}

void 
rf_update() {
	uint8_t data[7];
	cli();
	nrf_read_rx_payload(data,7);
	nrf_write_reg(NRF_REG_STATUS,0x40);
	if (data[1]==teamNumber) {
		switch (data[0]) {
			case PKT_ROBOT0:
			 robot0_x = (int16_t)((data[2]<<8) | data[3]);
			 robot0_y = (int16_t)((data[4]<<8) | data[5]);
			 break;	

			case PKT_ROBOT1:
			 robot1_x = (int16_t)((data[2]<<8) | data[3]);
			 robot1_y = (int16_t)((data[4]<<8) | data[5]);
			 break;

			case PKT_START:
			 roundState = data[2];
			 break;
		}
	}
	sei();
}

int16_t 
rf_get_x(uint8_t robot) {
	int16_t r;
	cli();
	if (robot)
		r = robot1_x;
	else
		r = robot0_x;
	sei();
	return r;
}

int16_t 
rf_get_y(uint8_t robot) {
	int16_t r;
	cli();
	if (robot)
		r = robot1_y;
	else
		r = robot0_y;
	sei();
	return r;
}

// rf thread
int 
rfread(void) {
	while (1) {
		/*
		if (rf_have_data())
			rf_update();
		*/
		pause(100);
	}

	//panic ("rfread");
	return 0;
}

