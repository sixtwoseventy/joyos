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

/**
 * \file global.h
 * \brief Miscellaneous kernel support functions.
 *
 * This header includes kernel-level support and debbugging functions as well
 * as JoyOS version information.
 */

//#include <assert.h>
#include <stddef.h>
#include "config.h" // TODO rewrite
#include <avr/pgmspace.h>

//#define __ASSERT_USE_STDERR
//#define assert(expr) if (!(expr)) { panic("assert"); } else

//void abort (void);

/**
 * System panic.
 * Call this routine to halt the board and display an error message on the
 * LCD. This routine never returns.
 *
 * @param msg	Display "panic: msg" on the LCD.
 */
void panic (char *msg);

void panic_P (PGM_P msg);

/**
 * Enable or disable the Auto-halt.
 * By default after 60 seconds, the kernel will disable all motors and servos
 * and halt. To disable this functionality call set_auto_halt(0);
 *
 * @param ah whether to enable or disable auto-halt functionality
 */
void set_auto_halt(uint8_t ah);

// throw compiler error if x is false
#define static_assert(x) switch(x) case 0: case (x):

//! Current JoyOS version
#define JOYOS_VERSION	"0.2.3"

#endif // __INCLUDE_GLOBAL_H__
