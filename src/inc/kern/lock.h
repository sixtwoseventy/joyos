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

#ifndef __INCLUDE_LOCK_H__
#define __INCLUDE_LOCK_H__

#ifdef SIMULATE
#include <pthread.h>
#endif

/**
 * \file lock.h
 * \brief Locks
 *
 * JoyOS includes a locking mechanism, useful when writing multi-threaded code.
 * Locks allow sections of code to be protected, requiring a thread to acquire
 * the lock before continuing. This allows multiple threads to to share a single
 * resource (such as a variable, or a sensor). All of the Happyboard hardware
 * drivers are locked to ensure thread-safe operation
 */

#ifndef SIMULATE
/// Maximum number of recursive acquires
#define LOCK_MAX_ACQUIRES 255 // protects against overflow
#endif

// lock structure
struct lock {
	#ifndef SIMULATE
    unsigned char locked;
    const char *name;
    struct thread *thread;
	#else
	pthread_mutex_t obj;
	#endif
};

/**
 * Initialize the lock 'k'.
 * This routine must be called before lock 'k' is used.
 *
 * @param k     Lock to initialize, must be non-null.
 * @param name  Debugging name for lock.
 */
void init_lock(struct lock *k, const char *name);

/**
 * Acquire the lock 'k'.
 * If another thread holds the lock yield until it is available, then take it.
 * A thread can recursively acquire the same lock multiple times, but it must
 * release once for every acquire.
 *
 * @param k Lock to acquire. Must be non-null.
 */
void acquire(struct lock *k);

/**
 * Try to acquire the lock 'k'.
 * If another thread holds the lock, return 0.
 * If the lock was acquired, return 1.
 *
 * @param k Lock to acquire. Must be non-null.
 */
int try_acquire(struct lock *k);

/**
 * Checks to see if lock 'k' is held by the current thread.
 *
 * @param k Lock to check. Must be non-null.
 * @return Non-zero if lock is held by current thread, zero otherwise.
 */
int is_held(struct lock *k);

/**
 * Release the lock 'k'.
 *
 * @param k Lock to release. Must be non-null.
 */
void release(struct lock *k);

/**
 * Smash the lock 'k', releasing it.
 * Should only ever be called by the kernel.
 *
 * @param k Lock to release. Must be non-null.
 */
void smash(struct lock *k);

#endif // __INCLUDE_LOCK_H__

