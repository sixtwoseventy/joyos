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

#ifndef _DIGITAL_H_
#define _DIGITAL_H_


/**
 * \file digital.h
 * \brief Digital Inputs.
 *
 * The happyboard has 8 digital inputs.
 * 
 * Reading the value of a digital input is acheived with digitalGetAll() or 
 * digitalGet()
 */ 

/** Initialize digital ports. Should not be called by user. */
void init_digital (void);

/**
 * Return the value of all of the digital ports as a single packed byte.
 * The least significant bit is port 0, the most significant is port 7.
 */
uint8_t digitalGetAll();

/**
 * Return the value of a specific digital port
 * @param port	port to read (0-7)
 */
uint8_t digitalGet(uint8_t port);

#endif
