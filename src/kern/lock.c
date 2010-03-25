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

// lock.c
//
// Basic lock implementation
//
// Written by: Greg Belote [gbelote@mit.edu]

#include <kern/global.h>
#include <kern/lock.h>
#include <kern/thread.h>
#include <avr/interrupt.h>

void init_lock(struct lock *k, const char *name) {
    // make sure k is non-null
    if (!k) {
        panic("init null lock");
    }

    k->locked = 0;
    k->name = name;
    k->thread = NULL;
}

uint8_t inc_lock(struct lock *k) {
    ATOMIC_BEGIN;
    extern struct thread *current_thread;
    if (!k->locked || k->thread==current_thread) {
        k->locked++;
        k->thread = current_thread;
        ATOMIC_END;
        return 1;
    } else {
        ATOMIC_END;
        return 0;
    }
}

void acquire(struct lock *k) {
    if (!k)
        panic("acquire null lock");

    if (k->locked >= LOCK_MAX_ACQUIRES)
        panic("acquired too many times");

    extern struct thread *current_thread;
    while (!inc_lock(k)) {
        if (current_thread != NULL)
            yield();
        else
            panic("deadlock in acquire -- called from kernel thread on unavailable lock");
    }
}

int try_acquire(struct lock *k) {
    if (!k)
        panic("acquire null lock");

    if (k->locked >= LOCK_MAX_ACQUIRES)
        panic("acquired too many times");

    return inc_lock(k);
}

// should only be called atomically
int is_held (struct lock *k) {
    if (!k)
        panic("is_held null lock");

    extern struct thread *current_thread;
    return current_thread == k->thread && k->locked;
}

void release(struct lock *k) {
    if (!k)
        panic("release null lock");

    ATOMIC_BEGIN;

    if (!is_held(k))
        panic("release unheld lock");

    if (!(--k->locked))
        k->thread = NULL;

    ATOMIC_END;
}

void smash(struct lock *k) {
    if (!k)
        panic("smash null lock");

    ATOMIC_BEGIN;

    k->locked = 0;
    k->thread = NULL;

    ATOMIC_END;
}
