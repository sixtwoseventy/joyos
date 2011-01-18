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

// inc/thread.h
//
// Basic thread implementation
//
// Written by: Greg Belote [gbelote@mit.edu]

#ifndef __INCLUDE_THREAD_H__
#define __INCLUDE_THREAD_H__

/**
 * \file thread.h
 * \brief Thread management.
 *
 * JoyOS includes a round-robbin pre-emptive scheduler, with a system tick of
 * 1ms.
 */

#ifndef SIMULATE
#include <setjmp.h>
#include <config.h>
#endif
#include <stdint.h>

#ifndef SIMULATE
#define TIMER_PRESCALER (64)
//#define TIMER_1MS_EXPIRE ((uint8_t)(0xff-(F_CPU/TIMER_PRESCALER/1000)+1))
#define TIMER_1MS_EXPIRE 131

// TODO probably defined elsewhere
#define SREG_IF 0x80
#endif

#define STACK_DEFAULT 256

#ifndef SIMULATE
#define ATOMIC_BEGIN uint8_t _cli_was_enabled = SREG & SREG_IF; cli();
#define ATOMIC_END SREG |= _cli_was_enabled;

struct jbuf {
    union {
        struct {
            uint8_t r2;
            uint8_t r3;
            uint8_t r4;
            uint8_t r5;
            uint8_t r6;
            uint8_t r7;
            uint8_t r8;
            uint8_t r9;
            uint8_t r10;
            uint8_t r11;
            uint8_t r12;
            uint8_t r13;
            uint8_t r14;
            uint8_t r15;
            uint8_t r16;
            uint8_t r17;
            uint16_t fp;
            uint16_t sp;
            uint8_t sreg;
            uint16_t pc;
        };
        jmp_buf jb;
    };
};

#define TO_JMP_BUF(jbuf) ((jbuf).jb)

struct thread {
    struct jbuf th_jmpbuf;
    uint16_t th_stacktop;
    uint16_t th_stacksize;
    uint8_t th_id;
    uint8_t th_status;
    uint8_t th_priority;
    uint32_t th_runs;
    uint32_t th_wakeup_time;
    void *th_channel;
    char *th_name;
    int (*th_func)();
};

// Macros for extracting jmp_buf thingies

#define JMPBUF_FP_OFFSET    16
#define JMPBUF_SP_OFFSET    18
#define JMPBUF_IP_OFFSET    21
#define JMPBUF_SREG_OFFSET  20

// TODO simplify expression...
#define JMPBUF_GET(jb, off, type)   (*((type *) (&((uint8_t *) &(jb))[off])))

#define JMPBUF_FP(jb)       ((jb).fp)
#define JMPBUF_SP(jb)       ((jb).sp)
#define JMPBUF_IP(jb)       ((jb).pc)
#define JMPBUF_SREG(jb)     ((jb).sreg)


enum {
    THREAD_FREE,
    THREAD_RUNNABLE,
    THREAD_NOT_RUNNABLE,
    THREAD_SLEEPING,
    THREAD_PAUSED,
};

#define MAX_THREADS 7

// setup multithreading
/**
 * Initialize multithreading. Should not be called by user.
 */
void init_thread(void);

/**
 * Schedule a thread to run. Should not be called by user.
 */
void schedule(void) __ATTR_NORETURN__;

/**
 * Load a thread and run it. Should not be called by user.
 *
 * @param t Thread to be run.
 */
void resume(struct thread *t) __ATTR_NORETURN__;

#endif

/**
 * Pause a thread for 'ms' milliseconds. The calling thread will give up the
 * processor until time expires. If interrupts are disabled, a busy loop will
 * be used. The pause is best-effort and may last several milliseconds longer
 * than specified. If threads pause frequently for short periods of time
 * (1-4ms, depending) thread starvation may occur.
 *
 * @param ms    Milliseconds to pause.
 */
void pause(uint32_t ms);

/**
 * Give up the processor to another thread until rescheduled.
 */
void yield(void);

/**
 * Terminate and exit the current thread.
 * This will free the thread's stack space, but not any of it's dynamically
 * allocated memory (malloc).
 */
#ifndef SIMULATE
void thread_exit(int) __ATTR_NORETURN__;
#else
void thread_exit(int);
#endif

/**
 * Create a new thread.
 * This routine will setup a new thread that will enter in 'func' and return
 * immediately. When 'func' returns, the new thread will terminate (via exit).
 *
 * @param func      Entry point of thread.
 * @param stacksize Size (in bytes) of new thread.
 * @param priority  Priority of new thread. 0 = highest, 255 = lowest
 * @param name      Name of thread, used for debugging.
 *
 * @return Thread ID of the new thread.
 */
uint8_t create_thread(int (*func)(), uint16_t stacksize, uint8_t priority, char *name);

/**
 * Stop everything.
 */
#ifndef SIMULATE
void halt(void) __ATTR_NORETURN__;
#else
void halt(void);
#endif

/**
 * Return the number of milliseconds elapsed since startup.
 */
uint32_t get_time (void);

long get_time_us (void);

#ifndef SIMULATE

void dump_jmpbuf(struct jbuf *);

/**
 * Output to the UART the state of every thread.
 */
void dump_threadstates ();

#endif

#endif // __INCLUDE_THREAD_H__

