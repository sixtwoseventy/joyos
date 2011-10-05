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

#ifndef _SERVO_H_
#define _SERVO_H_

#define SERVO_0     0
#define SERVO_1     1
#define SERVO_2     2
#define SERVO_3     3
#define SERVO_4     4
#define SERVO_5     5

/**
 * \file servo.h
 * \brief Servo outputs.
 *
 * The happyboard as 6 servo outputs.
 *
 * Each servo has a 9bit register used to set it's position. Setting the servo to 0 will
 * cause it to move to its counter-clockwise limit, while setting it to 511, will cause
 * it to move to its clockwise limit.
 */

/** Initialize servos. Should not be called by user. */
#ifndef SIMULATE
void servo_init(void);
#endif

/**
 * Set a servo's position.
 *
 * @param servo Servo to set.
 * @param pos   New servo value (0..511)
 */
void servo_set_pos(uint8_t servo, uint16_t pos);

/**
 * Disable servo - stops sending servo commands
 * until the next servo_set_pos();
 * 
 * @param servo Servo to disable
 */
void servo_disable(uint8_t servo);
void servo_set_range(uint8_t servo, uint16_t lower, uint16_t upper);

#endif
