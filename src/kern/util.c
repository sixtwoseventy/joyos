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

#include <avr/interrupt.h>
#include <board.h>
#include <kern/global.h>
#include <kern/util.h>
#include <kern/lock.h>
#include <lcd.h>
#include <avr/pgmspace.h>

void waitForClick(char *msg) {
    if (msg) {
#ifdef LCD_DEBUG
        // TODO: clean up, make more general (don't assume LCD)
        lcd_clear();
        // print to LCD
        lcd_printf("%s", msg);
#endif
        // print to UART
        uart_printf("%s\n", msg);
    }

    go_click();
}

// prints n bytes of 'bytes' to UART
void dumpBytes (uint8_t *bytes, uint8_t n) {
    uart_printf_P(PSTR("INDEX   ADDR   VALUE\n"));
    uart_printf_P(PSTR("====================\n"));
    for (int i = 0; i < n; i++) {
        uart_printf_P(PSTR("% 5d   %4p   %02x\n"),
                i, (void *)(bytes + i), bytes[i]);
    }
}

#endif 

