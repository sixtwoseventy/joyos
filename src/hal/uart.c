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
#include <kern/pipe.h>
#include <kern/ring.h>
#include <kern/lock.h>
#ifdef SIMULATE
#include <stdio.h>
#include <stdarg.h>
#endif

struct lock uart_lock;
#ifndef SIMULATE

// this routine could stand to read and write blocks larger than 1 byte
void uart_poll(pipe *p) {
    if (try_acquire(&p->rx_buf.lock)) {
        while ((UCSR0A & _BV(RXC0)) && (ring_size(&p->rx_buf) < p->rx_buf.capacity)) {
            uint8_t ch = UDR0;
            ring_write(&p->rx_buf, &ch, 1);
        }
        release(&p->rx_buf.lock);
    }
    if (try_acquire(&p->tx_buf.lock)) {
        while ((UCSR0A & _BV(UDRE0)) && (ring_size(&p->tx_buf) > 0)) {
            uint8_t ch;
            ring_read(&p->tx_buf, &ch, 1);
            UDR0 = ch;
        }
        release(&p->tx_buf.lock);
    }
}

void uart_init(uint16_t baudRate) {
    UBRR0L = (uint8_t)(F_CPU/(baudRate*16L)-1);
    UBRR0H = (F_CPU/(baudRate*16L)-1) >> 8;
    UCSR0A = 0x00;
    UCSR0C = 0x06;
    UCSR0B = _BV(TXEN0)|_BV(RXEN0);
}

#endif
