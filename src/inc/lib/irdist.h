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

#ifndef _IRDIST_H_
#define _IRDIST_H_

#ifndef SIMULATE
#include <kern/global.h>
#else
#include <joyos.h>
#endif

/**
 * \file irdist.h
 * \brief Sharp IR distance sensor linearisation
 */

/**
 * Set calibration values for IR distance sensors. Calibration values can be
 * calculated using the irdistcal program.
 *
 * @param m  calibration gradient
 * @param c  calibration offset
 */
void irdist_set_calibration(uint16_t m, uint16_t c);

/**
 * Read a Sharp IR distance sensor connected to an analog port and return
 * the distance in cm
 *
 * @param port  port to read from
 * @returns     measured distance in cm
 */
uint16_t irdist_read(int port);

#endif

