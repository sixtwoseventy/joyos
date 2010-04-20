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
#include "hal/spi.h"
#include <kern/lock.h>
#include <kern/global.h>

//static uint8_t in_use = 0;

struct lock spi_lock;

void spi_init (void) {
    init_lock(&spi_lock, "spi lock");
}

int8_t spi_acquire() {
    acquire(&spi_lock);
    return SPI_READY;
}

int spi_try_acquire() {
    return try_acquire(&spi_lock);
}

void spi_release () {
    release(&spi_lock);
}

void spi_set_master (spi_clk_div div, uint8_t flags) {
    // set spi control reg:
    // SPE - enable
    // MSTR - master mode
    // flags - see spi.h
    // div - high two bits of div are divides, low bit is 2X multiplier
    SPCR = _BV(SPE) | _BV(MSTR) | flags | (div>>1);
    // set multiplier flag
    if (div&1)
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

#endif
