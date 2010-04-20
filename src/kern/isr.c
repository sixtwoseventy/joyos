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

// Interrupt service routines
//
// Written by: Greg Belote [gbelote@mit.edu]

#include <avr/interrupt.h>
#include <kern/thread.h>
#include <kern/global.h>
#include <gyro.h>

extern uint32_t global_time;

ISR(TIMER2_OVF_vect) {
    TCNT2 = TIMER_1MS_EXPIRE;

    global_time++;

    yield();
}

ISR(__vector_default) {
    panic("unknown interrupt");
}

/**
 * Need a externally-referenced function in the object file containing
 * the ISRs to force the linker insert these ISRs into the IVT.
 * see http://www.nongnu.org/avr-libc/user-manual/library.html
 **/
void isr_init() {
}

#endif

