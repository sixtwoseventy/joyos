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

#ifndef __INCLUDE_GLOBAL_H__
#define __INCLUDE_GLOBAL_H__

#ifndef SIMULATE

/**
 * \file global.h
 * \brief Miscellaneous kernel support functions.
 *
 * This header includes kernel-level support and debbugging functions as well
 * as JoyOS version information.
 */

//#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "config.h" // TODO rewrite
#include <avr/pgmspace.h>

//#define __ASSERT_USE_STDERR
//#define assert(expr) if (!(expr)) { panic("assert"); } else

//void abort (void);

#endif

/**
 * System panic.
 * Call this routine to halt the board and write an error message to the
 * UART. This routine never returns.
 *
 * @param _x_   Write "panic: _x_" to the UART.
 */

#ifndef SIMULATE
#define panic(_x_) panic_P (PSTR(_x_))
#else
#define panic(_x_) panic_P (_x_)
#endif

#ifndef SIMULATE
void panic_P (PGM_P msg) __ATTR_NORETURN__;
#else
void panic_P (char* msg);
#endif

/**
 * Enables start of the round.  That is, if this is called, then the robot will
 * stop waiting or not wait at all to enter umain
 */
void round_start();

/**
 * Ends the round immediately and halts the robot
 */
void round_end();

// throw compiler error if x is false
#define static_assert(x) switch(x) case 0: case (x):


//! Current JoyOS version
#define JOYOS_VERSION "0.2.5"

#endif // __INCLUDE_GLOBAL_H__
