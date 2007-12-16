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
#include "hal/io.h"
#include "hal/delay.h"
#include "hal/adc.h"
#include "mcp3008.h"

/**
 * Battery voltage conversion math
 *
 * battery -> resistor divider -> 3.3V referenced 10bit adc
 * vbat = adc*v_adc*(r1+r2)*1000/(adc_res*r1)
 */
#define VBAT_R1   (4.7)
#define VBAT_R2   (10.0)
#define VBAT_ADC  (3.3)
#define VBAT_RES  (1024.0)
#define VBAT_MULT ((VBAT_ADC*(VBAT_R1+VBAT_R2)*1000.0)/(VBAT_RES*VBAT_R1))
#define VBAT_ADC_TO_MV(x) ((uint16_t)(((float)(x))*VBAT_MULT))

void 
go_click() {
	while (!SWITCH_GO());
	delay_busy_us(100);
	while (SWITCH_GO());
}

void 
stop_click() {
	while (!SWITCH_STOP());
	delay_busy_us(100);
	while (SWITCH_STOP());
}

uint8_t 
go_press() {
	return SWITCH_GO();
}

uint8_t 
stop_press() {
	return SWITCH_STOP();
}

uint16_t 
read_frob() {
	uint16_t v;
	adc_get_sample(ADC_REF_AVCC,ADC_CH1,&v);
	return v;
}

uint16_t 
read_battery() {
	uint16_t v;
	mcp3008_get_sample(MCP3008_MOTOR, MCP3008_CH7,&v);
	return VBAT_ADC_TO_MV(v);
}

void 
beep(uint16_t freq, uint16_t duration) {
	uint32_t count=0;
	uint16_t p = 2000000ul / freq;
	uint32_t i = p >> 2;
	while (count<(duration*300ul)) {
		BEEPER(1);
		delay_busy_us(p);
		BEEPER(0);
		delay_busy_us(p);
		count += i;
	}
}

