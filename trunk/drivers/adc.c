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
#include <global.h>
#include <lock.h>

#define PRESCALER	0x7

static uint8_t in_use = 0;
struct lock adc_lock;

void
init_adc (void) {
	init_lock(&adc_lock, "adc lock");
}

int8_t
adc_get_sample (adc_ref ref, adc_input config, uint16_t * val)
{
	acquire(&adc_lock);
	//uint8_t flags;

	//disable_interrupts (&flags);
	if (in_use) {
		//enable_interrupts (flags);
		return ADC_BUSY;
	}
	in_use = 1;
	//enable_interrupts (flags);

	ADMUX = (ref << 7) | _BV(REFS0) | /*_BV(ADLAR) |*/ config;
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADIF) | PRESCALER;

	while (!(ADCSRA & _BV(ADIF)));
	ADCSRA |= _BV(ADIF);

	*val = ADCL;
	*val |= (ADCH << 8);

	in_use = 0;

	/*
	if (config >= ADC_DIFF_0_0_10x && config <= ADC_DIFF_5_2) {
	// fill in the sign bits 
		*val = (*val << 6) >> 6;
	}
	*/

	release(&adc_lock);
	return ADC_SUCCESS;
}

static volatile uint8_t numsamples;
static uint8_t samplecount;
static volatile int16_t * samples;
static volatile adc_cb sample_cb;
static volatile void * userdata;
static adc_input adcconfig;

int8_t
adc_sample_async (adc_ref ref, adc_input config, uint8_t n,
		adc_cb cb, int16_t * vals, void * user)
{
	panic ("adc_sample_async");

	uint8_t flags;

	disable_interrupts (&flags);
	if (in_use) {
		enable_interrupts (flags);
		return ADC_BUSY;
	}
	in_use = 1;
	enable_interrupts (flags);

	adcconfig = config;
	samplecount = 0;
	numsamples = n;
	samples = vals;
	sample_cb = cb;
	userdata = user;

	ADMUX = (ref << 7) | _BV(REFS0) | _BV(ADLAR) | config;
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADIF) | _BV(ADIE) | PRESCALER;

	return ADC_SUCCESS;
}

SIGNAL(SIG_ADC)
{
	int16_t val;

	val = (uint8_t) ADCL;
	val |= ((uint8_t)ADCH) << 8;

	/*	if (adcconfig >= ADC_DIFF_0_0_10x && adcconfig <= ADC_DIFF_5_2) {
		// fill in the sign bits 
		val = (val << 6) >> 6;
	}
	*/

	samples[samplecount++] = val;
	if (samplecount < numsamples) {
		ADMUX = ADMUX + 1;
		ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADIE) | PRESCALER;
		return;
	}

	adc_cb cb = sample_cb;
	int16_t * s = (int16_t *) samples;
	uint8_t n = numsamples;
	void * user = (void *) userdata;
	in_use = 0;
	sei();

	cb (s, n, user);
}
