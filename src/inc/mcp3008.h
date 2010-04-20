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

#ifndef _MCP3008_H_
#define _MCP3008_H_

typedef enum {
    MCP3008_MOTOR,
    MCP3008_ADC1,
    MCP3008_ADC2,
} mcp3008_device;

typedef enum {
    MCP3008_CH0             = 0x8,
    MCP3008_CH1             = 0x9,
    MCP3008_CH2             = 0xa,
    MCP3008_CH3             = 0xb,
    MCP3008_CH4             = 0xc,
    MCP3008_CH5             = 0xd,
    MCP3008_CH6             = 0xe,
    MCP3008_CH7             = 0xf,
    MCP3008_DIFF_0_1        = 0x0,
    MCP3008_DIFF_1_0        = 0x1,
    MCP3008_DIFF_2_3        = 0x2,
    MCP3008_DIFF_3_2        = 0x3,
    MCP3008_DIFF_4_5        = 0x4,
    MCP3008_DIFF_5_4        = 0x5,
    MCP3008_DIFF_6_7        = 0x6,
    MCP3008_DIFF_7_6        = 0x7
} mcp3008_input;

#define MCP3008_SUCCESS     0
#define MCP3008_SPI_BUSY    -1

void init_mcp3008 (void);
int8_t mcp3008_get_sample (mcp3008_device dev, mcp3008_input config, uint16_t * sample);

#endif

#endif
