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

#ifndef _DELAY_H_
#define _DELAY_H_

#ifndef SIMULATE
#include <util/delay.h>
#else
#include <joyos.h>
#endif

/**
 * \file delay.h
 * \brief Simple busy-loop delays
 *
 * Some busy-loop delay functions. Because of the way threading works,
 * pause() is preferable to using these functions.
 */

/**
 * Wait for a certain number of milliseconds. Please use the pause() function instead.
 */
void delay_busy_ms(uint32_t ms);

/**
 * Wait for a certain number of microseconds. Please consider using pause() instead.
 */
void delay_busy_us(uint32_t us);

/// Wait in milliseconds
//#define delay_busy_ms(ms) (_delay_ms(ms))
/// Wait in in microseconds
//#define delay_busy_us(us) (_delay_us(us))

#endif
