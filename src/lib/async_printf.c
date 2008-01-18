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

#include <kern/global.h>
#include <kern/lock.h>
#include <kern/thread.h>
#include <lib/async_printf.h>
#include <lcd.h>
#include <hal/uart.h>
#include <hal/io.h>
#include <stdio.h>
#include <stdlib.h>
//#include <assert.h>

extern FILE lcdout;
extern FILE uartout;

typedef struct link_s link_t;
struct link_s {
	char *buf;
	FILE *out;
	link_t *next;
	link_t *prev;
};

static link_t *head = NULL;
static link_t *tail = NULL;

#define BUFFER_SIZE			33

struct lock print_queue_lock;

// add a link to the front of the queue
static void
_push_front (FILE *out, char *buf) {
	// create a new link structure
	link_t *link = (link_t *) malloc (sizeof (link_t));

	// record in new link the output and text
	link->out = out;
	link->buf = buf;

	// grab lock before modifying linked list
	acquire (&print_queue_lock);

	// set new link 'next' to head
	link->next = head;

	// if list was non-empty, set head 'prev' to new link
	if (head != NULL) {
		head->prev = link;
		// make sure we have a tail if list is non-empty
		//assert (tail != NULL);
	}

	else {
		// make sure we don't have a tail when the list is empty
		//assert (tail == NULL);
	}

	// set new link as head
	head = link;

	// if no tail, set as head/new link
	if (tail == NULL) {
		tail = link;
	}

	release (&print_queue_lock);
}

static link_t *
_pop_back () {
	acquire (&print_queue_lock);

	if (head == NULL) {
		//assert (tail == NULL);
		release (&print_queue_lock);
		return NULL;
	}

	//assert (tail != NULL);

	link_t *link = tail;
	if (tail->prev == NULL) {
		head = NULL;
		tail = NULL;
	}

	else {
		tail->prev->next = NULL;
		tail = NULL;
	}

	release (&print_queue_lock);

	return link;
}

int
async_loop_start (void) {
	init_lock(&print_queue_lock, "print queue");

	// loop and wait for input
	for (;;) {

		link_t *link = _pop_back ();

		if (link == NULL) {
			yield ();
		}

		else {
			if (link->out == &lcdout) {
				lcd_printf("%s", link->buf);
			} 

			else if (link->out == &uartout) {
				uart_printf("%s", link->buf);
			} 

			else {
				cli();
				uart_printf_P(PSTR("Unknown file descriptor %p\n"),link->out);
				uart_printf_P(PSTR(" uartout at %p\n"), &uartout);
				uart_printf_P(PSTR(" lcdout at %p\n"), &lcdout);
				panic_P (PSTR("async_loop"));
			}

			free (link->buf);
			free (link);
		} 
	}

	return 0;
}

int
async_vfprintf (FILE *out, const char *fmt, va_list ap) {
	char *buf;
	while((buf = (char *) malloc (BUFFER_SIZE)) == NULL) {
		panic_P (PSTR("async_vfprintf out of mem"));
		//yield();
	}

	// TODO this is a bug for out != lcdout
	int result = vsnprintf(buf, BUFFER_SIZE, fmt, ap);

	_push_front (out, buf);
	return result;
}

int
async_printf (FILE *out, const char *fmt, ...) {
	va_list ap;
	int count;

	if (out != &uartout && out != &lcdout) {
		panic_P (PSTR("unknown out"));
	}

	va_start(ap, fmt);
	count = async_vfprintf (out, fmt, ap);
	va_end(ap);

	return count;
}

