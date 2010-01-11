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

void
init_lock(struct lock *k, const char *name) {
	// make sure k is non-null
	if (!k) {
		panic("init null lock");
	}

	k->locked = 0;
	k->name = name;
	k->thread = NULL;
}

void 
acquire(struct lock *k) {
	// make sure k is non-null
	if (!k) {
		panic("acquire null lock");
	}

	if (k->locked >= LOCK_MAX_ACQUIRES) {
		panic("acquired too many times");
	}

	// loop until we acquire lock...
	for(;;) {
		// record interrupt status
		uint8_t was_enabled = SREG & SREG_IF;
		// disable global interrupts
		cli();

		// pointer to current thread
		extern struct thread *current_thread;
		// if current thread already holds lock
		if (k->thread == current_thread) {
			// inc lock counter
			k->locked++;
			// enable global interrupts (if previously enabled)
			SREG |= was_enabled;
			// done
			return;
		}

		// if not locked...
		if (!k->locked) {
			// lock it
			k->locked++;
			// record holding thread
			k->thread = current_thread;
			// enable global interrupts (if previously enabled)
			SREG |= was_enabled;
			// done
			return;
		} else {
			// enable global interrupts (if previously enabled)
			SREG |= was_enabled;
			// yield so holder can release
			yield();
		}
	}

	// control should never reach this
	panic("acquire");
}

int
is_held (struct lock *k) {
	//assert (k);
	if (!k) {
		panic("is_held null lock");
	}

	extern struct thread *current_thread;
	return current_thread == k->thread && k->locked;
}

void
release(struct lock *k) {
	// make sure k is non-null
	if (!k) {
		panic("release null lock");
	}

	// record interrupt status
	uint8_t was_enabled = SREG & SREG_IF;

	// disable global interrupts
	cli();

	// check lock is actually locked
	if (!k->locked) {
		panic("release unheld lock");
	}

	// check that lock is held by the current thread
	extern struct thread *current_thread;
	if (k->thread != current_thread) {
		panic("lock held by another");
	}

	// release lock
	k->locked--;

	// if lock is no longer held...
	if (!k->locked) {
		// remove pointer to holding thread
		k->thread = NULL;
	}

	// restore SREG (potentially reenabling interrupts)
	SREG |= was_enabled;
}

