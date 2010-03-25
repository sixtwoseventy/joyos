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

#ifndef _ENCODER_H_
#define _ENCODER_H_

/**
 * \file encoder.h
 * \brief Shaft Encoders.
 *
 * The Happyboard has 4 16-bit shaft encoders. The shaft encoders are fed
 * into the FPGA which manages the filtering and counting. The inputs are
 * filtered with a simple debouncing-filter to remove spurious counts.
 */

/** Initialize the encoders. Should not be called by user. */
void encoder_init (void);

/**
 * Reset the encoder count value for a specific shaft encoder
 * @param encoder encoder to reset (24-27)
 */
void encoder_reset(uint8_t encoder);

/**
 * Return the count value for a specific shaft encoder
 * @param encoder   encoder to read (24-27)
 */
uint16_t encoder_read(uint8_t encoder);

#endif
