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

#ifndef _ANALOG_H_
#define _ANALOG_H_


/**
 * \file analog.h
 * \brief Analog Inputs
 *
 * The Happyboard has 16 analog inputs. They are used for sensors such as gyroscopes,
 * distance sensors, IR phototransistor/LED pairs, and third-party sensors.
 * In JoyOS, the distance sensors and gyros have their own library code; do not use
 * analog_read() with these sensors. These inputs are fed into a 10
 * Bit ADC. The ADC communicates with the main processor via SPI, and
 * can do so at 500kHz (20k samples per second.) In reality the
 * actual maximum sample rate is slightly lower due to overheads in the OS code.
 * (Also, the bottleneck analog input sampling is usually processing code,
 * rather than the actual data acquisition).
 *
 * The analog, digital, and encoder inputs (as well as the LCD) are run off a
 * separate 5V regulated supply. This supply can supply up to 400mA to power
 * the input sensors.
 */

/// Maximum value for an analog reading
#define ANALOG_MAX  1023


/**
 * Read an analog port value.
 * @param port port number to read (8..23).
 * @return Value read by the analog port (0..1023)
 */
uint16_t analog_read(uint8_t port);

#endif
