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

#include "config.h"
#include "hal/uart_hboot.h"
#include "hal/io.h"

int uart_send(char ch) {
    LED_COMM(1);
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = ch;
    LED_COMM(0);
    return ch;
}

    void uart_print(const char *string) {
        while (*string)
            uart_send(*string++);
    }

char uart_recv() {
    LED_COMM(1);
    while(!(UCSR0A & _BV(RXC0)));
    LED_COMM(0);
    return (UDR0);
}

char uart_has_char() {
    return (UCSR0A & _BV(RXC0));
}

void uart_init(uint16_t baudRate) {
    UBRR0L = (uint8_t)(F_CPU/(baudRate*16L)-1);
    UBRR0H = (F_CPU/(baudRate*16L)-1) >> 8;
    UCSR0A = 0x00;
    UCSR0C = 0x06;
    UCSR0B = _BV(TXEN0)|_BV(RXEN0);
}
