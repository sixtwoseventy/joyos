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

#ifndef _IO_H_
#define _IO_H_

/**
 * \file io.h
 * \brief User Input/Output.
 *
 * This file contains various functions needed for the pushbuttons, frobknob, 
 * and beeper. 
 */ 


/** Initialize basic IO. Should not be called by user. */
void ioInit();

/**
 * Check if the 'Go' button is pressed.
 * @return true if go is pressed.
 */
uint8_t goPress();

/**
 * Check if the 'Stop' button is pressed.
 * @return true if stop is pressed.
 */
uint8_t stopPress();

/**
 * Wait for the user to click the 'Go' button.
 * This will loop until the 'Go' button is pressed and released.
 */
void goClick();

/**
 * Wait for the user to click the 'Stop' button.
 * This will loop until the 'Stop' button is pressed and released.
 */
void stopClick();

/**
 * Read a value from the Frob knob.
 * @return The position of the frob knob (value 0 to 1023).
 */
uint16_t readFrob();

/**
 * Read the battery voltage. Returns a value in decivolts.
 * @ return The battery voltage in decivolts.
 */
uint16_t readBattery();

/**
 * Beep at a given frequency for a given duration.
 *
 * @param freq		Frequency to beep
 * @param duration	Duration of beep
 */
void beep(uint16_t freq, uint16_t duration);

#endif
