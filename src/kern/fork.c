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


// Fork
//
// Split a thread into two.
//
// Written by: Greg Belote [gbelote@mit.edu]

#include <kern/thread.h>
#include <kern/global.h>
#include <avr/interrupt.h>
#include <setjmp.h>

// Duplicate this thread (with it's own stack).
// In the parent thread, this returns the thread id (tid) of the child.
// In the child thread, this returns 0.
int fork (void) {

    panic("fork");

    struct thread *child;

    // record interrupt status
    uint8_t was_enabled = SREG & SREG_IF;
    // lock thread table
    cli();

    // allocate new thread for child
    child = NULL; // FIXME

    // copy state to child thread
    //if (setjmp(child->th_jmpbuf) != 0) {
    if (setjmp(TO_JMP_BUF(child->th_jmpbuf)) != 0) {
        // we're the child now and we've just been scheduled
        // interrupts should be enabled

        // fork returns 0
        return 0;
    }

    // allocate child stack and copy contents of parent stack

    // enable child thread
    child->th_status = THREAD_RUNNABLE;

    // restore interrupt status
    SREG |= was_enabled;

    // we're the parent and our work is done, we're ready to return
    return child->th_id;
}
