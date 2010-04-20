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
#include "mcp3008.h"
#include "hal/io.h"
#include "hal/spi.h"
#include <kern/global.h>

int8_t mcp3008_get_sample(mcp3008_device dev, mcp3008_input config, uint16_t *sample) {
    uint8_t cmd[3];

    if (spi_acquire() == SPI_IN_USE)
        return MCP3008_SPI_BUSY;

    cmd[0] = 0x40 | (config << 2);

    spi_set_master (SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);

    switch (dev) {
        case MCP3008_MOTOR: SPI_MOTOR_SS(0); break;
        case MCP3008_ADC1:  SPI_ADC1_SS(0); break;
        case MCP3008_ADC2:  SPI_ADC2_SS(0); break;
    }
    spi_transfer_sync (cmd, 3);
    switch (dev) {
        case MCP3008_MOTOR: SPI_MOTOR_SS(1); break;
        case MCP3008_ADC1:  SPI_ADC1_SS(1); break;
        case MCP3008_ADC2:  SPI_ADC2_SS(1); break;
    }

    *sample = ((uint16_t)cmd[1] << 8) | cmd[2];
    *sample >>= 6;

    spi_release();

    return MCP3008_SUCCESS;
}

#endif

