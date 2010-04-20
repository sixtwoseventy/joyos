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
#include "config.h"
#endif
#include "hal/uart.h"
#ifndef SIMULATE
#include "hal/io.h"
#endif
#include <kern/lock.h>
#ifdef SIMULATE
#include <stdio.h>
#include <stdarg.h>
#endif

#ifndef SIMULATE

// setup LCD file descriptor (for printf)
//FILE uartout = FDEV_SETUP_STREAM(uart_put, NULL, _FDEV_SETUP_WRITE);
FILE uartio = FDEV_SETUP_STREAM(uart_put, uart_get, _FDEV_SETUP_RW);

struct lock uart_lock;

int uart_send(char ch) {
    LED_COMM(1);
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = ch;
    LED_COMM(0);
    return ch;
}

int uart_put(char ch, FILE *f) {
    if (ch == '\n')
        uart_send('\r');

    return uart_send(ch);
}

    void uart_print(const char *string) {
        while (*string)
            uart_send(*string++);
    }

int uart_vprintf(const char *fmt, va_list ap) {
    int count;
    acquire(&uart_lock);
    count = vfprintf(&uartio, fmt, ap);
    release(&uart_lock);

    return count;
}

#endif

int uart_printf(const char *fmt, ...) {
    va_list ap;
    int count;

    va_start(ap, fmt);
	#ifndef SIMULATE
    count = uart_vprintf(fmt, ap);
	#else
    count = vprintf(fmt, ap);
	#endif
    va_end(ap);

    return count;
}

#ifndef SIMULATE

int uart_vprintf_P(const char *fmt, va_list ap) {
    int count;
    acquire(&uart_lock);
    count = vfprintf_P(&uartio, fmt, ap);
    release(&uart_lock);

    return count;
}

int uart_printf_P(const char *fmt, ...) {
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = uart_vprintf_P(fmt, ap);
    va_end(ap);

    return count;
}

char uart_recv() {
    LED_COMM(1);
    while(!(UCSR0A & _BV(RXC0)));
    LED_COMM(0);
    return (UDR0);
}

int uart_get(FILE *f) {
    return uart_recv();
}

uint8_t uart_has_char() {
    return (UCSR0A & _BV(RXC0));
}

int uart_vscanf(const char *fmt, va_list ap){
    int count;
    acquire(&uart_lock);
    count = vfscanf(&uartio, fmt, ap);
    release(&uart_lock);

    return count;
}

#endif

int uart_scanf(const char *fmt, ...){
    va_list ap;
    int count;

    va_start(ap, fmt);
	#ifndef SIMULATE
    count = uart_vscanf(fmt, ap);
	#else
    count = vscanf(fmt, ap);
	#endif

    va_end(ap);

    return count;
}

#ifndef SIMULATE

int uart_vscanf_P(const char *fmt, va_list ap) {
    int count;
    acquire(&uart_lock);
    count = vfscanf_P(&uartio, fmt,ap);
    release(&uart_lock);

    return count;
}

int uart_scanf_P(const char *fmt, ...) {
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = uart_vscanf_P(fmt, ap);
    va_end(ap);

    return count;
}

void uart_init(uint16_t baudRate) {
    UBRR0L = (uint8_t)(F_CPU/(baudRate*16L)-1);
    UBRR0H = (F_CPU/(baudRate*16L)-1) >> 8;
    UCSR0A = 0x00;
    UCSR0C = 0x06;
    UCSR0B = _BV(TXEN0)|_BV(RXEN0);

    init_lock(&uart_lock, "UART lock");
}

#endif

