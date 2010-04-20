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

#ifndef SIMULATE

#ifndef __INCLUDE_MEMLAYOUT_H__
#define __INCLUDE_MEMLAYOUT_H__

/**
 * +----------------------+
 * |                      |
 * |                      |
 * |                      |
 * |     External RAM     |
 * |                      |
 * |                      |
 * |                      |
 * +======================+   <-- KSTACKTOP, RAMEND
 * |     Kernel stack     |
 * +----------------------+   <-- STACK_BLOCK_TOP, RAMEND - KSTACKSIZE
 * |    Thread 0 stack    |
 * +----------------------+   <-- (STACK_BLOCK_TOP - STACKSIZE * 1)
 * |    Thread 1 stack    |
 * +----------------------+
 * |         ....         |
 * +----------------------+   <-- (STACK_BLOCK_TOP - STACKSIZE * n)
 * |     Thread n stack   |
 * +----------------------+
 * |                      |
 * |      Heap space      |
 * |   malloc heap space  |
 * |                      |
 * +----------------------+
 * |                      |
 * |         .bss         |
 * |                      |
 * +----------------------+
 * |                      |
 * |        .data         |
 * |                      |
 * +----------------------+
 */

//#define STACKSIZE         400
#define STACKSIZE           300
#define KSTACKSIZE          328 // not needed?

//#define KSTACKTOP         0x1100
#define KSTACKTOP           0x10ff
#define STACK_BLOCK_TOP     (KSTACKTOP - KSTACKSIZE)

// Set STACK_SAFETY_ZONE bytes at the bottom of a thread's stack region to
// SAFETY_VALUE to help detect overflow.
#define STACK_SAFETY_ZONE   0x0
#define SAFETY_VALUE        0x42

#define STACKTOP(tid) (STACK_BLOCK_TOP - (STACKSIZE+STACK_SAFETY_ZONE) * (tid))

#endif

#endif // __INCLUDE_MEMLAYOUT_H__


