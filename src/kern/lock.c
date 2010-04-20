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

#ifndef SIMULATE

#include <kern/global.h>
#include <kern/lock.h>
#include <kern/thread.h>
#include <avr/interrupt.h>

#else

#include <pthread.h>
#include <joyos.h>

#endif

void init_lock(struct lock *k, const char *name) {
    // make sure k is non-null
    if (!k) {
        panic("init null lock");
    }

	#ifndef SIMULATE

    k->locked = 0;
    k->name = name;
    k->thread = NULL;

	#else

	pthread_mutex_init(&(k->obj), NULL);

	#endif

}

#ifndef SIMULATE

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

#endif

void acquire(struct lock *k) {
    if (!k)
        panic("acquire null lock");

	#ifndef SIMULATE

    if (k->locked >= LOCK_MAX_ACQUIRES)
        panic("acquired too many times");

    extern struct thread *current_thread;
    while (!inc_lock(k)) {
        if (current_thread != NULL)
            yield();
        else
            panic("deadlock in acquire -- called from kernel thread on unavailable lock");
    }

	#else

	pthread_mutex_lock (&(k->obj));
         
	#endif

}

int try_acquire(struct lock *k) {
    if (!k)
        panic("acquire null lock");

	#ifndef SIMULATE

    if (k->locked >= LOCK_MAX_ACQUIRES)
        panic("acquired too many times");

    return inc_lock(k);

	#else

	return pthread_mutex_trylock (&(k->obj)) == 0;

	#endif

}

#ifndef SIMULATE

// should only be called atomically
int is_held (struct lock *k) {
    if (!k)
        panic("is_held null lock");

    extern struct thread *current_thread;
    return current_thread == k->thread && k->locked;
}

#endif

void release(struct lock *k) {
    if (!k)
        panic("release null lock");

	#ifndef SIMULATE

    ATOMIC_BEGIN;

    if (!is_held(k))
        panic("release unheld lock");

    if (!(--k->locked))
        k->thread = NULL;

    ATOMIC_END;

	#else

    pthread_mutex_unlock(&(k->obj));

	#endif

}

#ifndef SIMULATE

void smash(struct lock *k) {
    if (!k)
        panic("smash null lock");

    ATOMIC_BEGIN;

    k->locked = 0;
    k->thread = NULL;

    ATOMIC_END;
}

#endif

