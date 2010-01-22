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

#ifndef __INCLUDE_GYRO_H__
#define __INCLUDE_GYRO_H__

/**
 * \file gyro.h
 * \brief Functions to retrieve heading information from the ADI gyroscope.
 */ 

/**
 * Initialize and calibrate the gyro located at `port'.
 *
 * @param port				Analog port number of the gyroscope.
 * @param lsb_us_per_deg	Constant for converting readings into degrees
 * @param time_ms			Time (in milliseconds) for calibrating gyro.
 */
void gyro_init (uint8_t port, float lsb_us_per_deg, uint32_t time_ms);

/**
 * Internal routine for updating gyroscope. Don't call this unless you know what
 * you're doing.
 */
int gyro_update (void);

float gyro_get_degrees (void);
void gyro_set_degrees (float deg);

#endif // __INCLUDE_GYRO_H__

