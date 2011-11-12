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
 * The Happyboard has 8 digital inputs.
 *
 * The digital inputs on the Happyboard are unfiltered and as such there is
 * "switch bounce" when they are connected to bump-sensors. To compensate for
 * this users should read digital inputs a few times with short delays between
 * readings to ensure accurate sensing.
 *
 */

/** Initialize digital ports. Should not be called by user. */
void digital_init (void);


/**
 * Return the value of all of the digital ports as a single packed byte.
 * The least significant bit is port 0, the most significant is port 7.
 */
uint8_t digital_read_8();

#ifndef SIMULATE

/**
 * Return the value of a specific digital port.
 * If the port specified is an analog port (8-23), digital_read() returns the
 * equivalent digital reading - <2.5V: FALSE >=2.5V: TRUE.
 * @param port  port to read (0-23)
 */
uint8_t digital_read(uint8_t port);

/**
 * Set the output of a specific digital port on or off
 * @param port  port to write (0-7)
 * @param on  0=off, 1=on
 */
void digital_write(uint8_t port, uint8_t on);


/**
 * Output PWM on a digital port
 * @param port  port to write (0-7)
 * @param val  PWM duty cycle [0-255]: 0=full off, 127=50% duty cycle, 255=full on
 */
void digital_write_pwm(uint8_t port, uint8_t val);



#endif

#endif
