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

#ifndef _ADC_H_
#define _ADC_H_

#define ADC_SUCCESS 0
#define ADC_BUSY    -1

typedef enum {
    ADC_REF_AVCC        = 0,
    ADC_REF_INTERNAL    = 1,
} adc_ref;

typedef enum {
    ADC_CH0             = 0x00,
    ADC_CH1             = 0x01,
    ADC_CH2             = 0x02,
    ADC_CH3             = 0x03,
    ADC_CH4             = 0x04,
    ADC_CH5             = 0x05,
    ADC_CH6             = 0x06,
    ADC_CH7             = 0x07,
    ADC_DIFF_0_0_10x    = 0x08,
    ADC_DIFF_1_0_10x    = 0x09,
    ADC_DIFF_0_0_200x   = 0x0A,
    ADC_DIFF_1_0_200x   = 0x0B,
    ADC_DIFF_2_2_10x    = 0x0C,
    ADC_DIFF_3_2_10x    = 0x0D,
    ADC_DIFF_2_2_200x   = 0x0E,
    ADC_DIFF_3_2_200x   = 0x0F,
    ADC_DIFF_0_1        = 0x10,
    ADC_DIFF_1_1        = 0x11,
    ADC_DIFF_2_1        = 0x12,
    ADC_DIFF_3_1        = 0x13,
    ADC_DIFF_4_1        = 0x14,
    ADC_DIFF_5_1        = 0x15,
    ADC_DIFF_6_1        = 0x16,
    ADC_DIFF_7_1        = 0x17,
    ADC_DIFF_0_2        = 0x18,
    ADC_DIFF_1_2        = 0x19,
    ADC_DIFF_2_2        = 0x1A,
    ADC_DIFF_3_2        = 0x1B,
    ADC_DIFF_4_2        = 0x1C,
    ADC_DIFF_5_2        = 0x1D,
    ADC_BG              = 0x1E,
    ADC_GND             = 0x1F
} adc_input;

typedef void (*adc_cb) (int16_t * samples, uint8_t n, void * user);

void adc_init (void);
int8_t adc_get_sample (adc_ref ref, adc_input config, uint16_t * val);

#endif

#endif

