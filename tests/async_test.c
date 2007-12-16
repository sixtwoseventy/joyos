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

#include <async_printf.h>
#include <uart.h>
#include <thread.h>
#include <lock.h>

extern FILE uartout;

int
print_parens (void) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 40; j++)
			async_printf(&uartout, "()");
		async_printf(&uartout, "\n");
	}
	for (;;);
}

int
async_test (void) {
	/*
	extern struct lock uart_lock;
	aquire(&uart_lock);
	uart_printf("async_test: uartout is %p\n", &uartout);
	release(&uart_lock);
	*/

	create_thread(&print_parens, STACK_DEFAULT, 0, "paren printer");

	for (int i = 0; i < 150; i++) {
		async_printf(&uartout, "x");
		yield();
	}

	for (;;);
	return 0;
}

