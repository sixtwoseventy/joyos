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

#ifndef SIMULATE
#include <board.h>
#include <buttons.h>
#include <kern/global.h>
#include <kern/memlayout.h>
#include <kern/thread.h>
#include <kern/thread.h>
#include <kern/util.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#else
#include <joyos.h>
#include <stdio.h>
#endif

// defined by user
extern int umain (void);
extern int usetup (void);

volatile uint8_t rf_start = 0;

void round_start() {
    rf_start = 1;
}

void round_end() {
    printf("\nRound end");
    halt();
}

int robot_monitor (void) {
    // start the calibration period...

    printf("Running usetup()...\n");
    usetup();
    printf("Finished usetup().\n");

    printf("Waiting for RF start, or press Go to start now.\n");
    while (!rf_start && !go_press())
      yield();

    printf("Running umain()...\n");
    return umain();
}

int main (void) {

    board_init();
	#ifdef SIMULATE
	init_socket();
	#endif

    printf("JoyOS v"JOYOS_VERSION"\n");

	#ifndef SIMULATE
    init_thread();
    create_thread(&robot_monitor, STACK_DEFAULT, 0, "main");
    rf_init();
    schedule();
	#else 
	robot_monitor();
	#endif

}
