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

#ifndef _SPI_H_
#define _SPI_H_

#define SPI_FLAG_LSB_FIRST	(1 << 0)
#define SPI_FLAG_MSB_FIRST	(0 << 0)
#define SPI_FLAG_HIGH_IDLE	(1 << 1)
#define SPI_FLAG_LOW_IDLE	(0 << 1)
#define SPI_FLAG_SAMP_TRAILING	(1 << 2)
#define SPI_FLAG_SAMP_LEADING	(0 << 2)
#define SPI_FLAG_DIV_CLK_4	(0 << 3)
#define SPI_FLAG_DIV_CLK_16	(1 << 3)
#define SPI_FLAG_DIV_CLK_64	(2 << 3)
#define SPI_FLAG_DIV_CLK_128	(3 << 3)
#define SPI_FLAG_MULT_CLK_2	(1 << 5)

#define SPI_READY	0
#define SPI_IN_USE	-1

typedef void (*spi_transfer_cb)(uint8_t * data, uint8_t len, void * user);

void init_spi(void);

/** Before any SPI transaction (sync or async), you must acquire the
    SPI bus. Upon success, then enable the appropriate slave select
    signal. Then, perform the transaction, release the slave select
    signal, and finally release the bus. **/
int8_t spi_acquire();
void   spi_release ();

/** If performing a SPI master transaction, call this function to
    configure the SPI mode AFTER acquiring the SPI bus. **/
void spi_set_master (uint8_t flags);
int8_t spi_transfer_sync (uint8_t * data, uint8_t len);
int8_t spi_transfer_async (uint8_t * data, uint8_t len,
		spi_transfer_cb cb, void * user);

#endif
