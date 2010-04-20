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
 *
 * The gyro is plugged into 4 of the analog ports on the Happyboard.
 * It should not be used with analog_read(); instead use the provided gyro
 * functions. Please see gyrotest for an example of correct gyro usage.
 * You will need to wait pause half a second before calling gyro_init (which
 * should be called in usetup).
 */

/**
 * Initialize and calibrate the gyro located at `port'.
 *
 * @param port              Analog port number of the gyroscope. This refers
 *                                      to the leftmost port of the 4 analog ports used.
 * @param lsb_us_per_deg    Constant for converting readings into degrees. Typical
 *                              value for gyro module Rev. B is 1400000.
 * @param time_ms           Time (in milliseconds) for calibrating gyro.
 *
 */
void gyro_init (uint8_t port, float lsb_us_per_deg, uint32_t time_ms);

/**
 * Internal routine for updating gyroscope. Don't call this unless you know what
 * you're doing.
 */
#ifndef SIMULATE
int gyro_update (void);
#endif

/**
 * Returns the current heading of an initialized gyroscope. This will not wrap;
 * in other words, if you turn counterclockwise twice, you will get a reading
 * of 720 degrees.
 */
float gyro_get_degrees (void);

/**
 * Sets the gyro to a specified degree. This is useful when used in conjuction
 * with PID controlled loops that turn specified number of degrees--you can
 * reset the gyro reading between distinct turning phases of nagivation.
 *
 * @param deg                          The degree value that you wish to set the gyro to read.
 */
#ifndef SIMULATE
void gyro_set_degrees (float deg);
#endif

#endif // __INCLUDE_GYRO_H__

