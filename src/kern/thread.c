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

// lib/thread.c
//
// Basic thread implementation
//
// Written by: Greg Belote [gbelote@mit.edu]

#include <kern/global.h>
#include <kern/thread.h>
#include <kern/util.h> // XXX: debugging
#include <kern/memlayout.h>
#include <hal/delay.h>
#include <hal/uart.h>
#include <hal/io.h>
#include <board.h>
#include <string.h>
#include <avr/interrupt.h>
#include <kern/lock.h>

// array of all threads
struct thread threads[MAX_THREADS];
// currently running thread
struct thread *current_thread = NULL;
volatile uint32_t global_time = 0;
struct jbuf sched_jbuf;

// idle thread -- should do nothing forever
int
idle(void) {
	for(;;) {
		yield();
	}

	panic ("idle");
	return 0;
}

void
setup_timer(void) {
	//TCCR2 = _BV(CS01) | _BV(CS00);
	TCCR2 = _BV(CS21) | _BV(CS20);
	TCNT2 = TIMER_1MS_EXPIRE;
	TIFR  |= _BV(TOV2);
	TIMSK |= _BV(TOIE2);
}

uint32_t
get_time (void) {
	uint32_t current_time;
	cli();
	current_time = global_time;
	sei();
	return current_time;
}

#define US_PER_TICK	((1000000UL*TIMER_PRESCALER)/F_CPU)

long
get_time_us (void) {
	long current_time;
	cli();
	//current_time = global_time * 1000 + 
	//	(uint32_t) (TCNT2 - TIMER_1MS_EXPIRE) * US_PER_TICK;
	current_time = global_time * 1000;
	sei();
	return current_time;
}

void
init_thread(void) {
	//extern uint16_t __bss_end;
	// make sure stack space won't collide with BSS space
	//if ((STACKTOP(MAX_THREADS) < __bss_end))
	//	panic ("no space for stacks");

	uart_printf_P (PSTR("thread table start at %p\n"), &threads);
	uart_printf_P (PSTR("thread table end at %p\n"), &threads[MAX_THREADS]);

	// set all threads as free
	int i;
	for (i = 0; i < MAX_THREADS; i++) {
		threads[i].th_id = i;
		threads[i].th_status = THREAD_FREE;
		threads[i].th_stacktop = STACKTOP(i);
		threads[i].th_runs = 0;
		uart_printf_P(PSTR("(id %d) stack [%p,%p)\n"),
				i, STACKTOP(i), STACKTOP(i+1));
	}

	// setup idle thread
	create_thread(&idle, STACK_DEFAULT, 0, "idle"); // XXX: priority?
	// assert idle thread is thread #0
	//assert (threads[0].th_status != THREAD_FREE);
	if (threads[0].th_status == THREAD_FREE)
		panic ("idle free");

	setjmp(TO_JMP_BUF(sched_jbuf));
	sched_jbuf.sp = KSTACKTOP;
	sched_jbuf.pc = (uint16_t) &schedule;

	setup_timer();
}

// Schedule a new thread to run
// assume interrupts disabled
void
schedule(void) {
	// wakeup any threads
	// give first priority to threads that should be unpaused...
	// note: we can starve if too many threads pause for very little time
	/*
	for (int i = 0; i < MAX_THREADS; i++) {
		if (threads[i].th_status == THREAD_PAUSED &&
				threads[i].th_wakeup_time <= global_time) {
			threads[i].th_status = THREAD_RUNNABLE;
			resume(&threads[i]);
		}
	}
	*/

	// get thread id
	int id;
	if (current_thread) {
		id = current_thread->th_id;
	} else
		id = 0;

	// for now, just do round robin...
	int offset;
	for (offset = 1; offset <= MAX_THREADS; offset++) {
		int tid = (id+offset) % MAX_THREADS;
		if (!tid) continue; // don't RR to idle

		if (threads[tid].th_status == THREAD_PAUSED &&
				threads[tid].th_wakeup_time <= global_time) {
			threads[tid].th_status = THREAD_RUNNABLE;
			resume(&threads[tid]);
		}

		// if next thread is runnable, resume
		if (threads[tid].th_status == THREAD_RUNNABLE) {
			resume(&threads[tid]); // run thread and enable interrupts
		} else if (threads[id].th_status == THREAD_PAUSED &&
				threads[id].th_wakeup_time <= global_time) {
			// unpause and go
			threads[id].th_status = THREAD_RUNNABLE;
			resume(&threads[tid]);
		}
	}

	if (threads[0].th_status == THREAD_RUNNABLE) {
		resume(&threads[0]);
	}

	cli();

	for (int i = 0; i < MAX_THREADS; i++) {
		if(threads[i].th_status == THREAD_FREE) {
			uart_printf_P(PSTR(" thread (tid %d) unallocated\n"), i);
			continue;
		}
		uart_printf_P(PSTR(" thread (tid %d) '%s' status %d\n"), 
				i, threads[i].th_name, threads[i].th_status);
		if(threads[i].th_status == THREAD_RUNNABLE) {
			uart_printf_P(PSTR(
					"Oops... there's a runnable thread thats not idle\n"));
		}
	}

	// should never return
	panic ("schedule");
}

void
check (struct thread *t) {
	if (threads[0].th_status != THREAD_RUNNABLE) {
		uart_printf_P(PSTR("-- no idle thread --\n"));
		uart_printf_P(PSTR("time %d"), global_time);
		uart_printf_P(PSTR("threads[0] at %p\n"), &threads[0]);
		uart_printf_P(PSTR("threads[0].th_status at %p\n"), 
				&(threads[0].th_status));

		extern uint16_t __malloc_heap_start, __malloc_heap_end;
		uart_printf_P(PSTR("malloc space [%p,%p]\n"),
				__malloc_heap_start, __malloc_heap_end);
		panic ("no idle");
	}

	// check if SP is above stacktop
	if (t->th_jmpbuf.sp > STACKTOP(t->th_id)) {
		panic ("SP above");
	}

	/*
	for (uint8_t i = 0; i < STACK_SAFETY_ZONE; i++) {
		uint8_t *sp = (uint8_t *) (t->th_stacktop - t->th_stacksize);
		if (*(sp-i) != SAFETY_VALUE) {
			uart_printf("\nstack overflow\n");
			uart_printf("safety value overwritten...\n");
			uart_printf("%p: %p\n", sp-i, *(sp-i));
			uart_printf("sp of '%s' (id %d) is %p\n", 
					t->th_name, t->th_id, t->th_jmpbuf.sp);
			dump_jmpbuf(&t->th_jmpbuf);
			uart_printf("stacktop: %p\n", t->th_stacktop);
			uart_printf("stacksize: %p\n", t->th_stacksize);
			uart_printf("reserved space: %p to %p\n",
					t->th_stacktop - t->th_stacksize, t->th_stacktop);
			panic ("stack overflow");
		}
	}
	*/


	// check if SP is below stacktop
	if (t->th_jmpbuf.sp <= STACKTOP(t->th_id+1)) {
		uart_printf_P(PSTR("\nstack overflow\n"));
		uart_printf_P(PSTR("sp of '%s' (id %d) is %p\n"),
				t->th_name, t->th_id, t->th_jmpbuf.sp);
		dump_jmpbuf(&t->th_jmpbuf);
		uart_printf_P(PSTR("pc at %p\n"), t->th_jmpbuf.pc);
		uart_printf_P(PSTR("stacktop: %p\n"), t->th_stacktop);
		uart_printf_P(PSTR("reserved space: %p to %p\n"),
				STACKTOP(t->th_id+1)+1, STACKTOP(t->th_id));
		panic ("stack overflow");
	}

	/*
	if (!(t->th_jmpbuf.sreg & SREG_IF)) {
		uart_printf("in thread '%s'...\n", t->th_name);
		dump_jmpbuf(&t->th_jmpbuf);
		panic ("IF clear");
	}
	*/
}

// assumes current_thread is non-null
// XXX: Trouble if threads are enabled ??
// Assumes interrupts are disabled...
void
suspend() {
	if (!current_thread)
		panic ("no current_thread");

	//if (setjmp(current_thread->th_jmpbuf) == 0)
	if (setjmp(TO_JMP_BUF(current_thread->th_jmpbuf)) == 0) {
		longjmp(TO_JMP_BUF(sched_jbuf),1);
		// set SP to kernel stack
		/*
		const uint8_t sp_h = (uint8_t) ((KSTACKTOP >> 8) % 256);
		const uint8_t sp_l = (uint8_t) (KSTACKTOP % 256);
		asm volatile(
				"mov __tmp_reg__, %0\n\t"
				"out __SP_H__, __tmp_reg__\n\t"
				"mov __tmp_reg__, %1\n\t"
				"out __SP_L__, __tmp_reg__\n\t" :: "I" (sp_h), "I" (sp_l));
		uint16_t ksp = (uint16_t) KSTACKTOP;
		asm volatile(
				"out __SP_H__, %A0\n\t"
				"out __SP_L__, %B0\n\t" :: "a" (ksp));

		// XXX TEST XXX
		struct jbuf testbuf;
		setjmp(TO_JMP_BUF(testbuf));
		if (testbuf.sp != KSTACKTOP)
			panic ("kstacktop");

		// make sure current thread is valid
		check(current_thread);
		schedule ();
		*/
	}
}

void
resume(struct thread *t) {
	current_thread = t;

	check(t); // XXX: debug!
	current_thread->th_runs++;
	longjmp(TO_JMP_BUF(current_thread->th_jmpbuf), 1);

	// resume should never return
	panic ("resume");
}

void
pause(uint16_t ms) {
	// if interrupts are disabled...
	if (!(SREG & SREG_IF)) {
		// spin for a while
		delay_busy_ms(ms);
		return;
	}

	cli();

	current_thread->th_status = THREAD_PAUSED;
	// XXX assumes clock period is 1 ms
	current_thread->th_wakeup_time = global_time + ms;

	yield();

	// at this point interrupts are reenabled
}

void
yield(void) {
	//if(!(SREG & SREG_IF)) panic ("bad yield");
	cli();

	// store registers not in jmp_buf on stack
	asm volatile(
			"push r1\n\t"
			"push r0\n\t"
			"push r18\n\t"
			"push r19\n\t"
			"push r20\n\t"
			"push r21\n\t"
			"push r22\n\t"
			"push r23\n\t"
			"push r24\n\t"
			"push r25\n\t"
			"push r26\n\t"
			"push r27\n\t"
			"push r30\n\t"
			"push r31\n\t" ::);

	//assert (current_thread);
	if (!current_thread) panic ("yield");

	suspend();

	// restore registers stored above
	asm volatile(
			"pop r31\n\t"
			"pop r30\n\t"
			"pop r27\n\t"
			"pop r26\n\t"
			"pop r25\n\t"
			"pop r24\n\t"
			"pop r23\n\t"
			"pop r22\n\t"
			"pop r21\n\t"
			"pop r20\n\t"
			"pop r19\n\t"
			"pop r18\n\t"
			"pop r0\n\t"
			"pop r1\n\t" ::);

	sei();
}

void 
sleep(void *channel) {
	// disable global interrupts
	//lock_threadtable();
	cli();

	//assert(current_thread);
	if (!current_thread) panic ("sleep");

	current_thread->th_channel = channel;
	current_thread->th_status = THREAD_SLEEPING;

	// yield processor to another (runnable) process
	yield();
}

void
wakeup(void *channel) {
	// lock thread array
	//lock_threadtable();
	cli();

	int i;
	for (i = 1; i < MAX_THREADS; i++)
		if (threads[i].th_status == THREAD_SLEEPING)
			if (threads[i].th_channel == channel)
				threads[i].th_status = THREAD_RUNNABLE;

	// unlock thread array
	//release_threadtable();
	sei();
}

void
exit(int status) {
	for (;;) yield();
	// stop everything
	cli();

	//assert(current_thread);
	if (!current_thread)
		panic ("exiting nothing");

	current_thread->th_status = THREAD_FREE;
	yield();

	panic ("exit");
	for (;;); // satisfy compiler
}

uint16_t
//allocate_stack(uint16_t size) {
allocate_stack(uint16_t size, uint8_t tid) { // TODO refactor
	return STACKTOP(tid);





	/*
	uint8_t *sp = (uint8_t *) (STACKTOP(tid+1)+1);

	// write safety values
	for (uint8_t i = 0; i < STACK_SAFETY_ZONE; i++) {
		// *(sp++) = SAFETY_VALUE;
		//uart_printf("wrote safety value %p at %p\n",
		//		*(sp-1), sp-1);
		sp++;
	}
	// move pointer to end of block (stack top)
	//sp += size-1;
	//uart_printf("STACKTOP(%d) = %p\n", tid, STACKTOP(tid));
	sp += STACKSIZE-1;
	//uart_printf("allocate_stack stacktop = %p\n", (uint16_t) sp);
	//_delay_ms(1);

	return (uint16_t) sp;
	//return STACKTOP(tid);
	*/
}

uint8_t
create_thread(int (*func)(), uint16_t stacksize, uint8_t priority, char *name) {
	//lock_threadtable();
	uint8_t was_enabled = SREG & SREG_IF;
	cli();

	int i;
	for (i = 0; i < MAX_THREADS; i++)
		if (threads[i].th_status == THREAD_FREE) {
			threads[i].th_status = THREAD_RUNNABLE;
			threads[i].th_name = name;
			threads[i].th_runs = 0;

			threads[i].th_stacksize = STACKSIZE;

			// set initial state of the thread's registers...
			// TODO: best?
			//memset(&threads[i].th_jmpbuf, 0, sizeof(jmp_buf));
			memset(&threads[i].th_jmpbuf, 0, sizeof(struct jbuf));

			// calculate the thread's stack top
			//uint8_t *sp = (uint8_t *) threads[i].th_stacktop;
			uint8_t *sp = (uint8_t *) allocate_stack(0, i); // XXX refactor
			//uart_printf("allocated stacktop = %p\n", sp);

			// push argc and argv onto the stack and set registers
			// TODO implement

			// push a return address to exit onto the stack
			*(sp--) = (uint8_t) (&exit);
			*(sp--) = (uint8_t) ((uint16_t) &exit >> 8);

			// set the stack
			threads[i].th_jmpbuf.sp = (uint16_t) sp;

			// set the thread's instruction pointer
			threads[i].th_jmpbuf.pc = (uint16_t) func;

			// set interrupt enable flag
			threads[i].th_jmpbuf.sreg |= SREG_IF;

			// enable global interrupts (if previously enabled)
			SREG |= was_enabled;
			return i;
		}

	panic ("out of threads");
	for(;;); // satisfy compiler
}

void
halt(void) {
	// stop interrupts
	cli();

	// nuke motor lock
	extern struct lock motor_lock;
	motor_lock.locked = 0;
	motor_lock.thread = NULL;

	// break each motor
	for (uint8_t i = 0; i < 6; i++)
		motor_brake (i);

	// enter busy loop forever
	for(;;);

	// unreachable code, should never happen
	panic ("halt");
}

/*
void
dump_jmpbuf(jmp_buf *jb) {
	uart_printf("Dumping jmp_buf:\n");

	uint8_t *bytes = (uint8_t *) jb;
	// print r2-r17
	for (int i = 0; i < 16; i++) {
		uart_printf(" r%02u  : 0x%02x\n", i+2, (uint8_t) bytes[2]);
	}

	// print frame pointer
	uart_printf(" FP   : 0x%04x\n", JMPBUF_FP(jb));

	uart_printf(" SP   : 0x%04x\n", JMPBUF_SP(jb));

	uart_printf(" SREG : 0x%02x\n", JMPBUF_SREG(jb));

	uart_printf(" PC   : 0x%04x\n", JMPBUF_IP(jb));
}
*/

void
dump_jmpbuf(struct jbuf *jb) {
	/*
	uart_printf("Dumping jmp_buf:\n");

	uart_printf("r0\tNA\t\tr1\tNA\t\tr2\t%2p\t\tr3\t%2p\t\tr4\t%2p\n",
			jb->r2, jb->r3, jb->r4);
	uart_printf("r5\t%2p\t\tr6\t%2p\t\tr7\t%2p\t\tr8\t%2p\t\tr9\t%2p\n",
			jb->r5, jb->r6, jb->r7, jb->r8, jb->r9);
	uart_printf("r10\t%2p\t\tr11\t%2p\t\tr12\t%2p\t\tr13\t%2p\t\tr14\t%2p\n",
			jb->r10, jb->r11, jb->r12, jb->r13, jb->r14);
	uart_printf("r15\t%2p\t\tr16\t%2p\t\tr17\t%2p\t\tr18\t%2p\t\tr19\t%2p\n",
			jb->r15, jb->r16, jb->r17, jb->r18, jb->r19);
	uart_printf("r20\t%2p\t\tr21\t%2p\t\tr22\t%2p\t\tr23\t%2p\t\tr24\t%2p\n",
			jb->r20, jb->r21, jb->r22, jb->r23, jb->r24);
	uart_printf("r25\t%2p\t\tr26\t%2p\t\tr27\t%2p\t\tr28\tNA\t\tr29\tNA\n",
			jb->r25, jb->r26, jb->r27);
	uart_printf("r30\t%2p\t\tr31\t%2p\n",
			jb->r30, jb->r31);

	uart_printf(" FP   : %04p\n", (uint32_t) JMPBUF_FP(*jb));
	uart_printf(" SP   : %04p\n", (uint32_t) JMPBUF_SP(*jb));
	uart_printf(" SREG : %02p\n", (uint32_t) JMPBUF_SREG(*jb));
	uart_printf(" PC   : %08p\n", (uint32_t) JMPBUF_IP(*jb));
	*/
}

void
dump_threadstates () {
	uint8_t sreg = SREG & SREG_IF;
	cli();

	uart_printf_P(PSTR("Dumping thread states:\n"));

	for (int i = 0; i < MAX_THREADS; i++) {
		if(threads[i].th_status == THREAD_FREE) {
			uart_printf_P(PSTR(" thread (tid %d) unallocated\n"), i);
			continue;
		}
		/*
		uart_printf(" thread (tid %d) '%s' status %d runs %u\n", 
				i, threads[i].th_name, threads[i].th_status, 
				threads[i].th_runs);
		*/
		uart_printf_P(PSTR(" thread (tid %d) '%s' status %d runs %u\n"),
				i, threads[i].th_name, threads[i].th_status,
				threads[i].th_runs);
	}

	SREG |= sreg;
}

int
display_thread_states (void) {
	while (1) {
		dump_threadstates ();
		pause (3000);
	}

	return 0;
}

