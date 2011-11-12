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

#ifndef SIMULATE

#include <kern/global.h>
#include <kern/thread.h>
#include <kern/util.h> // XXX: debugging
#include <kern/memlayout.h>
#include <hal/delay.h>
#include <hal/io.h>
#include <board.h>
#include <string.h>
#include <avr/interrupt.h>
#include <kern/lock.h>

#else

#include <joyos.h>
#include <pthread.h>
#include <time.h>

#endif

#ifndef SIMULATE

extern struct lock uart_lock;

// array of all threads
struct thread threads[MAX_THREADS];
// currently running thread
struct thread *current_thread = NULL;
volatile uint32_t global_time = 0;
struct jbuf sched_jbuf;

void setup_timer(void) {
    TCCR2 = _BV(CS21) | _BV(CS20);
    TCNT2 = TIMER_1MS_EXPIRE;
    TIFR  |= _BV(TOV2);
    TIMSK |= _BV(TOIE2);
}

#define US_PER_TICK ((1000000UL*TIMER_PRESCALER)/F_CPU)

void init_thread(void) {
    ATOMIC_BEGIN;

    //extern uint16_t __bss_end;
    // make sure stack space won't collide with BSS space
    //if ((STACKTOP(MAX_THREADS) < __bss_end))
    //  panic("no space for stacks");

    printf ("thread table start at %p\n", &threads);
    printf ("thread table end at %p\n", &threads[MAX_THREADS]);

    // set all threads as free
    int i;
    for (i = 0; i < MAX_THREADS; i++) {
        threads[i].th_id = i;
        threads[i].th_status = THREAD_FREE;
        threads[i].th_stacktop = STACKTOP(i);
        threads[i].th_runs = 0;
        printf("(id %d) stack [%p,%p)\n",
                i, STACKTOP(i), STACKTOP(i+1));
    }

    setjmp(TO_JMP_BUF(sched_jbuf));
    sched_jbuf.sp = KSTACKTOP;
    sched_jbuf.pc = (uint16_t) &schedule;

    setup_timer();

    ATOMIC_END;
}

// Schedule a new thread to run
// assume interrupts disabled
void schedule(void) {
    int id = current_thread ? current_thread->th_id : MAX_THREADS-1;
    current_thread = NULL;

    int offset;
    for (offset = 1; offset <= MAX_THREADS; offset++) {
        int tid = (id+offset) % MAX_THREADS;

        if (threads[tid].th_status == THREAD_PAUSED &&
                threads[tid].th_wakeup_time <= global_time)
            threads[tid].th_status = THREAD_RUNNABLE;

        if (threads[tid].th_status == THREAD_RUNNABLE)
            resume(&threads[tid]);
    }

    // wait for aliens to take us home...
    SREG |= SREG_IF;
    for(;;);
}

void check (struct thread *t) {
    // printfs need the uart lock to be free, and
    // we're definitely going to panic if we do any
    // printfs at all, so we will smash the uart lock
    // if we have to.

    // check if SP is above stacktop
    if (t->th_jmpbuf.sp > STACKTOP(t->th_id)) {
        panic("SP above");
    }

    // check is SP is below bottom (stacktop-stacksize)
    if (t->th_stacktop - t->th_jmpbuf.sp > t->th_stacksize) {
        smash(&uart_lock);
        printf("\nstack overflow\n");
        printf("sp of '%s' (id %d) is %p\n",
                t->th_name, t->th_id, t->th_jmpbuf.sp);
        dump_jmpbuf(&t->th_jmpbuf);
        printf("pc at %p\n", t->th_jmpbuf.pc);
        printf("stacktop: %p\n", t->th_stacktop);
        printf("reserved space: %p to %p\n",
                t->th_stacktop-t->th_stacksize+1, t->th_stacktop);
        panic("stack overflow");
    }

    /*
    // check if SP is below stacktop
    if (t->th_jmpbuf.sp <= STACKTOP(t->th_id+1)) {
        smash(&uart_lock);
        printf("\nstack overflow\n");
        printf("sp of '%s' (id %d) is %p\n",
                t->th_name, t->th_id, t->th_jmpbuf.sp);
        dump_jmpbuf(&t->th_jmpbuf);
        printf("pc at %p\n", t->th_jmpbuf.pc);
        printf("stacktop: %p\n", t->th_stacktop);
        printf("reserved space: %p to %p\n",
                STACKTOP(t->th_id+1)+1, STACKTOP(t->th_id));
        panic("stack overflow");
    }
*/

    /*
    for (uint8_t i = 0; i < STACK_SAFETY_ZONE; i++) {
        smash(&uart_lock);
        uint8_t *sp = (uint8_t *) (t->th_stacktop - t->th_stacksize);
        if (*(sp-i) != SAFETY_VALUE) {
            printf("\nstack overflow\n");
            printf("safety value overwritten...\n");
            printf("%p: %p\n", sp-i, *(sp-i));
            printf("sp of '%s' (id %d) is %p\n",
                    t->th_name, t->th_id, t->th_jmpbuf.sp);
            dump_jmpbuf(&t->th_jmpbuf);
            printf("stacktop: %p\n", t->th_stacktop);
            printf("stacksize: %p\n", t->th_stacksize);
            printf("reserved space: %p to %p\n",
                    t->th_stacktop - t->th_stacksize, t->th_stacktop);
            panic("stack overflow");
        }
    }
    */

    /*
    if (!(t->th_jmpbuf.sreg & SREG_IF)) {
        smash(&uart_lock);
        printf("in thread '%s'...\n", t->th_name);
        dump_jmpbuf(&t->th_jmpbuf);
        panic("IF clear");
    }
    */
}

void resume(struct thread *t) {
    current_thread = t;

    check(t);
    current_thread->th_runs++;
    longjmp(TO_JMP_BUF(current_thread->th_jmpbuf), 1);
}

#endif

long get_time_us (void) {

	#ifndef SIMULATE

    ATOMIC_BEGIN;

    long current_time;
    current_time = global_time * 1000 +
        (uint32_t) (TCNT2 - TIMER_1MS_EXPIRE) * US_PER_TICK;

    ATOMIC_END;

    return current_time;

	#else

	return (long)(1000000. * ((float)clock()) / ((float)CLOCKS_PER_SEC));

	#endif
}

uint32_t get_time (void) {

	#ifndef SIMULATE
	
    ATOMIC_BEGIN;

    uint32_t current_time;
    current_time = global_time;

    ATOMIC_END;

    return current_time;

	#else

	return (uint32_t)(1000. * ((float)clock()) / ((float)CLOCKS_PER_SEC));

	#endif

}

void pause(uint32_t ms) {

	#ifndef SIMULATE

    // if interrupts are disabled...
    if (!(SREG & SREG_IF)) {
        // spin for a while
        delay_busy_ms(ms);
        return;
    } else {
        ATOMIC_BEGIN; // yes, I know interrupts are enabled, but...

        current_thread->th_status = THREAD_PAUSED; // don't change your thread buf with interrupts enabled!!!111~~
        current_thread->th_wakeup_time = global_time + ms;

        yield();

        ATOMIC_END;
    }

	#else
	
	delay_busy_ms(ms);

	#endif

}

void yield(void) {

	#ifndef SIMULATE

    ATOMIC_BEGIN;

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

    if (!current_thread ||
            setjmp(TO_JMP_BUF(current_thread->th_jmpbuf)) == 0)
        longjmp(TO_JMP_BUF(sched_jbuf),1);

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

    ATOMIC_END;

	#else

	pthread_yield();

	#endif

}

void thread_exit(int status) {

	#ifndef SIMULATE

    ATOMIC_BEGIN;

    if (!current_thread)
        panic("exiting nothing");

    current_thread->th_status = THREAD_FREE;
    yield();

    panic("scheduled thread that had already exited");
    ATOMIC_END;

	#else 

	pthread_exit(NULL);

	#endif
}

#ifndef SIMULATE

uint16_t allocate_stack(uint16_t size, uint8_t tid) {
    return STACKTOP(tid);

    /*
    uint8_t *sp = (uint8_t *) (STACKTOP(tid+1)+1);

    // write safety values
    for (uint8_t i = 0; i < STACK_SAFETY_ZONE; i++) {
    // *(sp++) = SAFETY_VALUE;
    //printf("wrote safety value %p at %p\n",
    // *(sp-1), sp-1);
    sp++;
    }
    // move pointer to end of block (stack top)
    //sp += size-1;
    //printf("STACKTOP(%d) = %p\n", tid, STACKTOP(tid));
    sp += STACKSIZE-1;
    //printf("allocate_stack stacktop = %p\n", (uint16_t) sp);
    //_delay_ms(1);

    return (uint16_t) sp;
    //return STACKTOP(tid);
     */
}

#endif

#ifndef SIMULATE

void thread_stub(void) {

    SREG |= SREG_IF;
    thread_exit(current_thread->th_func());

}

#else

void* thread_stub(void* arg){
	
	// cast argument to a function pointer and call it
	(*((int (*)())arg))();

}

#endif

uint8_t create_thread(int (*func)(), uint16_t stacksize, uint8_t priority, char *name) {

	#ifndef SIMULATE

    ATOMIC_BEGIN;

    int i;
    for (i = 0; i < MAX_THREADS; i++)
        if (threads[i].th_status == THREAD_FREE)
            break;

    if (i == MAX_THREADS)
        panic("out of threads");

    threads[i].th_status = THREAD_RUNNABLE;
    threads[i].th_name = name;
    threads[i].th_runs = 0;
    threads[i].th_stacksize = STACKSIZE;
    threads[i].th_func = func;

    // make the jmp_buf something sensible,
    // calculate the thread's stack top, and
    // set the thread's instruction pointer
    setjmp(TO_JMP_BUF(threads[i].th_jmpbuf));
    threads[i].th_jmpbuf.sp = (uint16_t)allocate_stack(0, i);;
    threads[i].th_jmpbuf.pc = (uint16_t)thread_stub;

    ATOMIC_END;
    return i;

	#else

    pthread_t tid;
    pthread_create(&tid, NULL, thread_stub, (void*)func);

	#endif
}

void halt(void) {

	uint8_t i;

	#ifndef SIMULATE

    // stop interrupts
    cli();

    // nuke motor lock
    extern struct lock motor_lock;
    smash(&motor_lock);
    extern struct lock servo_lock;
    smash(&servo_lock);

	#endif

    // brake each motor
    for (i = 0; i < 6; i++) {
        motor_brake (i);
        servo_disable(i);
    }


    // enter busy loop forever
    while(1) {
        for (i = 0; i < 3; i++) {
            LED_COMM(1);
            pause(100);
            LED_COMM(0);
            pause(150);
        }
        pause(800);
    }
}

#ifndef SIMULATE

void dump_jmpbuf(struct jbuf *jb) {
    printf("Dumping jmp_buf:\n");

    printf (" <omitting r0-r31>\n");

    printf(" FP   : %04p\n", (uint32_t) JMPBUF_FP(*jb));
    printf(" SP   : %04p\n", (uint32_t) JMPBUF_SP(*jb));
    printf(" SREG : %02p\n", (uint32_t) JMPBUF_SREG(*jb));
    printf(" PC   : %08p\n", (uint32_t) JMPBUF_IP(*jb));
}

void dump_threadstates () {
    acquire(&uart_lock);
    ATOMIC_BEGIN;

    printf("Dumping thread states:\n");

    for (int i = 0; i < MAX_THREADS; i++) {
        if(threads[i].th_status == THREAD_FREE) {
            printf(" thread (tid %d) unallocated\n", i);
            continue;
        }

        printf(" thread (tid %d) '%s' status %d runs %u\n",
                i, threads[i].th_name, threads[i].th_status,
                threads[i].th_runs);
    }

    ATOMIC_END;
    release(&uart_lock);
}

int display_thread_states (void) {
    while (1) {
        dump_threadstates ();
        pause (3000);
    }

    return 0;
}

#endif

