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

#include <avr/interrupt.h>
#include <lcd.h>
#include <kern/global.h>
#include <kern/lock.h>
#include <kern/thread.h>
#include <avr/pgmspace.h>

#define PANIC_STRING "\npanic: %S\n"
void
panic_P (PGM_P msg) {
	// stop everything
	cli();
    // notify user
#ifdef LCD_DEBUG
	extern struct lock lcd_lock;

	// clobber LCD lock so we can write
    smash(&lcd_lock);

	lcd_clear(); // clear lcd
	// report panic message
	lcd_printf_P(PSTR(PANIC_STRING), msg ? msg : ""); // print msg
#endif
	extern struct lock uart_lock;

    smash(&uart_lock);

	printf(PANIC_STRING, msg ? msg : "");

	// halt board
	halt();
}

