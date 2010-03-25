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

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

/** Bits of precision for the frob knob. */
#define FROB_BITS   (10)
/** Min value of frob knob. */
#define FROB_MIN    (0)
/** Max value of frob knob. */
#define FROB_MAX    (1 << FROB_BITS)

/**
 * \file buttons.h
 * \brief User Input/Output - buttons, frob, beeper and battery.
 *
 * This file contains various functions needed for the pushbuttons, frobknob,
 * and beeper (found in previous revisions of the happyboard).
 * It also allows the user to read the battery voltage.
 */

/**
 * Check if the 'Go' button is pressed.
 * @return true if go is pressed.
 */
uint8_t go_press();

/**
 * Check if the 'Stop' button is pressed.
 * @return true if stop is pressed.
 */
uint8_t stop_press();

/**
 * Wait for the user to click one of the two buttons.
 * Returns 0 if Stop was pressed.
 */
int either_click();

/**
 * Wait for the user to click the 'Go' button.
 * This will loop until the 'Go' button is pressed and released.
 */
void go_click();

/**
 * Wait for the user to click the 'Stop' button.
 * This will loop until the 'Stop' button is pressed and released.
 */
void stop_click();

/**
 * Read a value from the Frob knob.
 * @return The position of the frob knob (value 0 to 1023).
 */
uint16_t frob_read();

/**
 * Read the from the frob and scale into the range min<=frob<=max
 * @param min  minimum scaled value
 * @param max  maximum scaled value
 * @return     return the scaled position of the frob
 */
uint16_t frob_read_range(uint16_t min, uint16_t max);

/**
 * Read the battery voltage. Returns a value in decivolts.
 * @ return The battery voltage in decivolts.
 */
uint16_t read_battery();

/**
 * Beep at a given frequency for a given duration.
 *
 * @param freq      Frequency to beep
 * @param duration  Duration of beep
 */
void beep(uint16_t freq, uint16_t duration);

#endif
