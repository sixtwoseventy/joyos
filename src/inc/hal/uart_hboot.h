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

#ifndef _UART_H_
#define _UART_H_

/**
 * \file uart.h
 * \brief UART (USB serial port) functionality.
 *
 * The Happyboard has a FT232RL USB serial bridge chip on board, which makes it
 * appear as a serial port on most OS's (Windows/Mac/Linux). Users can print
 * information to the UART and monitor it on a computer using a terminal
 * emulator program such as hyperterminal or minicom.
 *
 * The Happyboard configures the UART for 19200 baud, 8N1.
 */

/**
 * Send a character over UART.
 */
int uart_send(char ch);

/**
 *
 */
char uart_recv();

/**
 *
 */
char uart_has_char();

/**
 * Initialize the UART driver.
 */
void uart_init(uint16_t baudRate);

#endif


