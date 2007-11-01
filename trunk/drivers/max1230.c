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

#include "spi.h"
#include "max1230.h"

#include <lock.h>

#define FLAGS	(SPI_FLAG_MSB_FIRST | SPI_FLAG_LOW_IDLE | \
		SPI_FLAG_SAMP_LEADING | SPI_FLAG_DIV_CLK_4)

struct lock max1230_lock;

uint8_t max1230Init() {
	init_lock(&max1230_lock, "max1230");
	uint8_t cmd[1];
	
	if (spi_acquire() == SPI_IN_USE)
		return MAX1230_SPI_BUSY;

	spi_set_master(FLAGS);

	cmd[0] = 0x64;

	SPI_ADC_SS(0);
	spi_transfer_sync(cmd, 1);
	SPI_ADC_SS(1);
	_delay_us(1);

	spi_release();

	return MAX1230_READY;

}

uint8_t max1230SingleReading(Max1230Channel chan, uint16_t *val) {
	acquire(&max1230_lock);
	uint8_t	cmd[1];
	uint8_t msb;

	if (spi_acquire() == SPI_IN_USE) {
		release(&max1230_lock);
		return MAX1230_SPI_BUSY;
	}

	spi_set_master(FLAGS);
	
	cmd[0] = 0x64;

	SPI_ADC_SS(0);
	spi_transfer_sync(cmd, 1);
	SPI_ADC_SS(1);
	_delay_us(1);

	cmd[0] = (0x80 | chan | MAX1230_SCAN_NONE);

	SPI_ADC_SS(0);
	spi_transfer_sync(cmd, 1);
	SPI_ADC_SS(1);
	
	_delay_us(500);
	
	cmd[0] = 0;
	SPI_ADC_SS(0);
	spi_transfer_sync(cmd, 1);
	SPI_ADC_SS(1);

	msb = cmd[0];
	_delay_us(1);

	cmd[0] = 0;
	SPI_ADC_SS(0);
	spi_transfer_sync(cmd, 1);
	SPI_ADC_SS(1);
	_delay_us(1);

	spi_release();

	*val = (msb<<8) | cmd[0];

	release(&max1230_lock);

	return MAX1230_READY;
}
