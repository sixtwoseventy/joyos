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

#ifndef SIMULATE

#include <kern/thread.h>
#include <kern/global.h>
#include <avr/interrupt.h>
#include <setjmp.h>
#include <string.h>

extern struct thread *current_thread;
extern struct thread threads[MAX_THREADS];

// Duplicate this thread (with it's own stack).
// In the parent thread, this returns the thread id (tid) of the child.
// In the child thread, this returns 0.
int fork (void) {

    panic("fork");
    // pause scheduling
    ATOMIC_BEGIN;

    // allocate new thread for child
    int i = create_thread(current_thread->th_func, current_thread->th_stacksize, current_thread->th_priority, current_thread->th_name);

    uint16_t child_sp, parent_sp, size;

    // get stack pointers
    parent_sp = SP;
    size = current_thread->th_stacktop - parent_sp;
    child_sp = threads[i].th_stacktop - size;

    // copy stack to child
    memcpy((void*)(child_sp + 1), (const void *)(parent_sp + 1), size);

    // patch child jump buffer
    threads[i].th_jmpbuf.sp = child_sp;
    threads[i].th_jmpbuf.pc = (uint16_t)&&label_ret;

    ATOMIC_END;
    return i; // under what conditions might this be zero?
label_ret:
    ATOMIC_END;
    return 0;
}

#endif

