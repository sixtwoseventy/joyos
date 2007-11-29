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
#include "adc.h"

void ioInit() {
	// XMEM
	MCUCR |= _BV(SRE);
	XMCRB &= ~_BV(XMBK);
	XMCRA &= ~_BV(SRW11);
	MCUCR &= ~_BV(SRW10);

	// Port B: SPI, Beeper, FPGA conf
	DDRB = 0xF7; 
	PORTB |= _BV(6);
	// PORT D: I2C, switches, leds
	DDRD = 0xCC; 
	PORTD |= _BV(4) | _BV(5);
	// PORTE: uart, lcd
	DDRE = 0xFE;
	// PORTF: ADC, JTAG
	DDRF = 0x04;
	// PORTG: mem, FPGA conf
	DDRG = 0x00;

	// disable all SPI devices
	SPI_FLASH_SS(1);
	SPI_MOTOR_SS(1);
	SPI_ADC_SS(1);
	SPI_RF_SS(1);
}

void goClick() {
	while (!SWITCH_GO());
	_delay_us(100);
	while (SWITCH_GO());
}

void stopClick() {
	while (!SWITCH_STOP());
	_delay_us(100);
	while (SWITCH_STOP());
}

uint8_t goPress() {
	return SWITCH_GO();
}

uint8_t stopPress() {
	return SWITCH_STOP();
}

uint16_t readFrob() {
	uint16_t v;
	adc_get_sample(ADC_REF_AVCC,ADC_CH1,&v);
	return v;
}

uint16_t readBattery() {
	uint16_t v;
	adc_get_sample(ADC_REF_AVCC,ADC_CH0,&v);
	return (uint16_t)(((float)v)/10.3);
}

void beep(uint16_t freq, uint16_t duration) {
	uint32_t count=0;
	uint16_t p = 2000000ul / freq;
	uint32_t i = p >> 2;
	while (count<(duration*300ul)) {
		BEEPER(1);
		_delay_loop_2(p);
		BEEPER(0);
		_delay_loop_2(p);
		count += i;
	}
}

