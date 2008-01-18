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
#include <hal/uart.h>
#include <kern/global.h>
#include <kern/memlayout.h>
#include <kern/thread.h>
#include <kern/thread.h>
#include <kern/util.h>
#include <lib/async_printf.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// defined by user
extern int umain (void);
extern int idle (void);
extern int usetup (void);

static uint8_t auto_halt = 1;

void set_auto_halt(uint8_t ah) {
	auto_halt = ah;
}

int robot_monitor (void) {
	// start the calibration period...
	usetup ();

	//lcd_printf_P (PSTR("\nWaiting for RF  'Go' to override"));
	// wait for the start signal
	// FIXME: add new start code
	//while (rf_get_round_state() != STATE_RUNNING && !go_press ())
	//	yield ();

	//lcd_printf ("\nStarting umain()");
	//pause (100);
	// create a user thread
	create_thread(&umain, STACK_DEFAULT, 0, "user main"); // XXX: priority?
	//dump_threadstates ();

	// TODO: code to detect all stop
	/*
	uint32_t start_time = get_time();
	while (get_time() - start_time < 60000)
		yield();
	*/
	if (!auto_halt) {
		while (1) { yield(); }
	} else {
		pause (60000L);
		lcd_printf_P (PSTR("\nRound end"));
		halt();
	}

	return 0;
}

int main (void) {
	// check for runaway code
	extern struct thread *current_thread;
	if (current_thread != NULL)
		panic_P (PSTR("runaway code"));

	// startup malloc
	extern uint16_t __malloc_heap_end;
	__malloc_heap_end = STACKTOP(MAX_THREADS);

	// startup board
	board_init();

	extern uint16_t __malloc_heap_start;
	uart_printf_P (PSTR("__malloc_heap_start = %p\n"), __malloc_heap_start);
	uart_printf_P (PSTR("__malloc_heap_end = %p\n"), __malloc_heap_end);

	int16_t heap_size = 2 * (__malloc_heap_end - __malloc_heap_start);
	uart_printf_P (PSTR("heap size: %d bytes\n"), heap_size);

	if (__malloc_heap_start >= __malloc_heap_end)
		panic_P (PSTR("         memory full"));

	// TODO check for debug mode

	PGM_P boot_msg = PSTR("JoyOS v"JOYOS_VERSION"    Press GO");
	lcd_printf_P (boot_msg);
	uart_printf_P (boot_msg);
	uart_printf_P (PSTR("\n"));

	// startup multithreading
	init_thread();

	//create_thread(&idle, STACK_DEFAULT, 0, "init2");

	// create the async printf
	//create_thread(&async_loop_start, 0, "async printf");

	// create the rf thread
	//create_thread(&rfread, STACK_DEFAULT, 0, "rf"); // XXX: priority?

	// create a thread to manage calibration and starting
	create_thread(&robot_monitor, STACK_DEFAULT, 0, "robot monitor");

	// start scheduling
	schedule();
	//extern struct jbuf sched_jbuf;
	//longjmp(TO_JMP_BUF(sched_jbuf),1);

	// control should never reach here
	panic_P (PSTR("main"));

	return 0;
}

