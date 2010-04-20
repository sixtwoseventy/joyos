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

#ifndef _SPI_H_
#define _SPI_H_

#define SPI_FLAG_LSB_FIRST      _BV(DORD)
#define SPI_FLAG_MSB_FIRST      0
#define SPI_FLAG_IDLE_HIGH      _BV(CPOL)
#define SPI_FLAG_IDLE_LOW       0
#define SPI_FLAG_SAMPLE_TRAIL   _BV(CPHA)
#define SPI_FLAG_SAMPLE_LEAD    0

#define SPI_FLAGS_DEFAULT (SPI_FLAG_MSB_FIRST | SPI_FLAG_IDLE_LOW | SPI_FLAG_SAMPLE_LEAD)

typedef enum {
    SPI_CLK_DIV_2   = 1, // 001
    SPI_CLK_DIV_4   = 0, // 000
    SPI_CLK_DIV_8   = 3, // 011
    SPI_CLK_DIV_16  = 2, // 010
    SPI_CLK_DIV_32  = 5, // 101
    SPI_CLK_DIV_64  = 4, // 100
    SPI_CLK_DIV_128 = 6, // 110
} spi_clk_div;

#define SPI_READY   0
#define SPI_IN_USE  -1

void spi_init(void);

/**
 * Lock the SPI bus for access from a single thread.
 * spi_aquire() needs to be called before any communication on the
 * spi bus. spi_release() should be called after the communication
 * is complete.
 */
int8_t spi_acquire();
void   spi_release ();

/** If performing a SPI master transaction, call this function to
    configure the SPI mode AFTER acquiring the SPI bus. **/
void spi_set_master(spi_clk_div div, uint8_t flags);

int8_t spi_transfer_sync (uint8_t * data, uint8_t len);
int spi_try_acquire();

#endif

#endif

