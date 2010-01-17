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

// lib/main.c
//
// C-entry of JoyOS. Should setup multithreading and whatnot, then create
// a user thread (calling umain()).
//
// Written by: Greg Belote [gbelote@mit.edu]

#include <board.h>
#include <buttons.h>
#include <kern/global.h>
#include <kern/memlayout.h>
#include <kern/thread.h>
#include <kern/thread.h>
#include <kern/util.h>
#include <lib/async_printf.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

//#define EXTERNAL_RAM=1

// defined by user
extern int umain (void);
extern int usetup (void);

static uint8_t auto_halt = 0;
//static uint8_t auto_round_start = 0;

void set_auto_halt(uint8_t ah) {
	auto_halt = ah;
}

volatile uint8_t rf_start = 0;

void round_start() {
    rf_start = 1;
}

void round_end() {
	printf("\nRound end");
	halt();
}

void wait_for_rf (void) {
    while (!rf_start)
        yield();
}

int robot_monitor (void) {
	// start the calibration period...

	printf("\nRunning usetup()...");
	usetup();
	printf("\nFinished usetup().");

	printf("\nPress Go to start or Stop to wait for RF.");
    if (!either_click())
        wait_for_rf();

	printf("\nRunning umain()...");
	return umain();
}

int main (void) {
	// check for runaway code
	extern struct thread *current_thread;
	if (current_thread != NULL)
		panic("runaway code");

	// startup malloc

#ifndef EXTERNAL_RAM
	__malloc_heap_end = (void*)STACKTOP(MAX_THREADS);
#else
	__malloc_heap_start = 0x8000;
	__malloc_heap_end = 0xFFFF;
#endif
	// startup board
	board_init();

	printf ("__malloc_heap_start = %p\n", __malloc_heap_start);
	printf ("__malloc_heap_end = %p\n", __malloc_heap_end);

	uint16_t heap_size = (__malloc_heap_end - __malloc_heap_start);
	printf("heap size: %u bytes\n", heap_size);

	if (__malloc_heap_start >= __malloc_heap_end)
		panic("         memory full");

	printf("JoyOS v"JOYOS_VERSION"\n");

	// initialize threads and timer
	init_thread();

	// create a thread to manage calibration and starting
	create_thread(&robot_monitor, STACK_DEFAULT, 0, "main");

    // initialize RF
    rf_init();

	// start scheduling (doesn't return)
	schedule();

	return 0;
}
