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
#include "at45db011.h"

uint8_t fpga_init(uint16_t start, uint16_t len) {
    uint16_t outSize = 0;
    uint16_t binSize;
    uint8_t val, i, bit, count;
    uint8_t obuf[8];

    while (at45db_start_continuous_read(start) == AT45DB_SPI_BUSY);

    // pulse prog_b line to start programming
    FPGA_PROGB(0);
    delay_busy_us(100);
    FPGA_PROGB(1);

    // get binary size
    //i = at45db_get_next_byte();
    //binSize = (i<<8) + at45db_get_next_byte();
    binSize = len;

    while (outSize<binSize) {
        count = 1;
        // get next byte
        val = at45db_get_next_byte();
        outSize++;
        // is rle?
        if ((val & 0xC0) == 0xC0) {
            // yes, update count, get value
            count = 0x3F & val;
            val = at45db_get_next_byte();
            outSize++;
        }
        // generate bit array
        for (bit=0;bit<8;bit++)
            obuf[bit] = val & (1<<(7-bit));
        // output stream
        for (i=0;i<count;i++) {
            for (bit=0;bit<8;bit++) {
                FPGA_DIN(obuf[bit]);
                FPGA_CCLK(1);
                FPGA_CCLK(0);
            }
        }
    }

    at45db_end_continuous_read();

    // return configuration status
    return FPGA_DONE();
}

#endif

