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

#ifndef MOTOR_GROUP_H
#define MOTOR_GROUP_H

#include <kern/global.h>

/**
 * \file motor_group.h
 * \brief A grouping of motors as single unit
 *
 * A motor group is a collection of motors that act as a single unit. This is
 * useful in the situation where many motors are connected to single output
 * shaft. A motor group allows these motors to be treated as a single entity.
 */

typedef uint8_t MotorGroup; ///< MotorGroup

/// Check if a group contains a motor
#define motor_group_has(group, motor) ((group)&(1<<(motor)))

#define MOTOR_GROUP_ALL 0x3F

/**
 * Create a new motor group and specify which motors are in the group.
 * For example, a group with motors 1,2 and 3:
 * \code
 * MotorGroup m123 = motor_group_new(0,1,1,1,0,0);
 * \endcode
 */
MotorGroup motor_group_new(bool m0, bool m1, bool m2, bool m3, bool m4, bool m5);

/**
 * Set the velocity of each motor in the group
 *
 * @param group   group specifying which motors to set
 * @param vel     motor velocity
 */
void motor_group_set_vel(MotorGroup group, int16_t vel);

/**
 * Brake each motor in a group
 *
 * @param group   group specifying which motors to brake
 */
void motor_group_brake(MotorGroup group);

#endif
