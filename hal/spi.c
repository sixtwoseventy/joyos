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
#include <avr/interrupt.h>
#include <lock.h>
#include <global.h>

#include "spi.h"

//static uint8_t in_use = 0;

struct lock spi_lock;

void init_spi (void) {
	init_lock(&spi_lock, "spi lock");
}

int8_t spi_acquire() {
	/*
	uint8_t cr;

	disable_interrupts (&cr);
	if (in_use) {
		enable_interrupts (cr);
		return SPI_IN_USE;
	}
	in_use = 1;
	enable_interrupts (cr);
	*/
	acquire(&spi_lock);
	return SPI_READY;
}

void spi_release () {
	//in_use = 0;
	release(&spi_lock);
}

void spi_set_master (uint8_t flags) {
	uint8_t cr;

	cr = _BV(SPE) | _BV(MSTR);

	if (flags & SPI_FLAG_LSB_FIRST)
		cr |= _BV(DORD);
	if (flags & SPI_FLAG_HIGH_IDLE)
		cr |= _BV(CPOL);
	if (flags & SPI_FLAG_SAMP_TRAILING)
		cr |= _BV(CPHA);
	if (flags & SPI_FLAG_DIV_CLK_16)
		cr |= _BV(SPR0);
	if (flags & SPI_FLAG_DIV_CLK_64)
		cr |= _BV(SPR1);
	
	/* set MOSI, SCK, and SS to outputs */
	DDRB |= _BV(PB2) | _BV(PB1) | _BV(PB0);
	/* set the SPI control register */
	SPCR = cr;

	if (flags & SPI_FLAG_MULT_CLK_2)
		SPSR |= _BV(SPI2X);
	else
		SPSR &= ~_BV(SPI2X);
}

int8_t spi_transfer_sync (uint8_t * data, uint8_t len) {
	uint8_t i;

	for (i = 0; i < len; i++) {
		SPDR = data[i];
		while (!(SPSR & _BV(SPIF)));
		data[i] = SPDR;
	}
	return 0;
}

static uint8_t data_len;
static uint8_t count;
static uint8_t * databuf;
static spi_transfer_cb transfer_cb;
static void * user_data;

int8_t spi_transfer_async (uint8_t * data, uint8_t len, spi_transfer_cb cb,
		void * user) {
	panic ("spi_transfer_sync");

	if (len <= 0)
		return 0;

	data_len = len;
	count = 0;
	databuf = data;
	transfer_cb = cb;
	user_data = user;

	SPCR |= _BV(SPIE);
	SPDR = data[0];
	return 0;
}

SIGNAL(SIG_SPI) {
	databuf[count++] = SPDR;
	if (count < data_len) {
		/* proceed with next byte of SPI transfer */
		SPDR = databuf[count];
	}
	else {
		/* we are done */
		SPCR &= ~_BV(SPIE);
		sei ();
		transfer_cb (databuf, data_len, user_data);
	}
}

