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

#include "config.h"
#include "hal/io.h"
#include "hal/delay.h"
#include "hal/adc.h"
#include "mcp3008.h"
#include <buttons.h>

#else

#include <joyos.h>
#include <stdio.h>

#endif

#ifndef SIMULATE

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

#endif

int either_click() {
	
	#ifndef SIMULATE

    int which = 0;
    while (!SWITCH_GO() && !SWITCH_STOP());
    which = SWITCH_GO();
    delay_busy_us(100);
    while (SWITCH_GO() || SWITCH_STOP());
    return which;

	#else

	printf("Returning \"GO\" from either_click()...\n");

	return 1;

	#endif

}

void go_click() {

	#ifndef SIMULATE

    while (!SWITCH_GO());
    delay_busy_us(100);
    while (SWITCH_GO());

	#else

	printf("Skipping go_click()...\n");

	#endif

}

void stop_click() {

	#ifndef SIMULATE

    while (!SWITCH_STOP());
    delay_busy_us(100);
    while (SWITCH_STOP());

	#else

	printf("Skipping stop_click()...\n");

	#endif

}

uint8_t go_press() {

	#ifndef SIMULATE

    return SWITCH_GO();

	#else

	printf("Returning 1 from go_press()...\n");
	return 1;

	#endif

}

uint8_t stop_press() {

	#ifndef SIMULATE

    return SWITCH_STOP();

	#else

	printf("Returning 1 from stop_press()...\n");
	return 1;

	#endif	

}

uint16_t frob_read() {

	#ifndef SIMULATE

    uint16_t v;
    adc_get_sample(ADC_REF_AVCC,ADC_CH1,&v);
    return v;
	
	#else

	printf("Returning 0 from frob_read()...\n");
	return 0;

	#endif

}

uint16_t frob_read_range(uint16_t min, uint16_t max) {
//    uint16_t div = max-min+1;
//    return (frob_read()/div) + min;

	#ifndef SIMULATE

    uint16_t range = max-min+1;
    return (uint16_t) (((uint32_t)frob_read()*range)/FROB_MAX + min);

	#else

	printf("Returning %d from frob_read_range(%d, %d)...\n", min, min, max);
	return min;

	#endif

}

uint16_t read_battery() {

	#ifndef SIMULATE

    uint16_t v;
    mcp3008_get_sample(MCP3008_MOTOR, MCP3008_CH7,&v);
    return VBAT_ADC_TO_MV(v);

	#else

	printf("Returning 8000 from read_battery()...\n");
	return 8000;

	#endif

}

void beep(uint16_t freq, uint16_t duration) {

	#ifndef SIMULATE
	
    uint32_t count=(((uint32_t)duration)*freq)/1000ul;
    uint16_t p = 500000ul / freq;
    while (count--) {
        BEEPER(1);
        delay_busy_us(p);
        BEEPER(0);
        delay_busy_us(p);
    }

	#endif

}

