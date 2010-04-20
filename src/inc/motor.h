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

#ifndef _MOTOR_H_
#define _MOTOR_H_

#ifndef SIMULATE
#define MOTOR_0     0
#define MOTOR_1     1
#define MOTOR_2     2
#define MOTOR_3     3
#define MOTOR_4     4
#define MOTOR_5     5
#endif

/**
 * \file motor.h
 * \brief Motor Outputs.
 *
 * The happyboard as 6 1.5A Motor drivers with current sensing capabilities.
 * Motors can be set to 3 possible states: \em drive, \em coast, or \em brake.
 *
 *  - \em Drive is the default state, used for any velocity other than 0
 *  - \em Coast is used when a motor is set to 0 velocity. This causes the motor to coast
 *          to a stop (the motor is not actively stopped).
 *  - \em Brake is used when motorBrake is called, and actively stops the motor by shorting the
 *          motor terminals.
 *
 * Each motor port has a current sensor associated with it, allowing the robot to sense the
 * current usage of each motor. Motor current increases proportional to torque and thus can
 * be used to sense when a motor has stalled (driving into a wall, other robots, etc).
 */

/** Initialize motors. Should not be called by user. */
void motor_init(void);

/**
 * Set the motor velocity of a specific motor. The sign of 'vel' determines
 * direction. A value of 0 sets the motor to coast.
 *
 * @param motor Motor port to set.
 * @param vel   Velocity to set motor 'motor'.
 */
void motor_set_vel(uint8_t motor, int16_t vel);

/**
 * Set a motor to brake (stops the motor as quickly as possible).
 * Calling motor_set_vel() again will disable the brake state.
 *
 * @param motor Motor port to brake.
 */
#ifndef SIMULATE
void motor_brake(uint8_t motor);
#endif

/**
 * Read the current in a given motor.
 *
 * @param motor Motor to sense current of.
 * @return the current of motor 'motor'.
 */
#ifndef SIMULATE
uint16_t motor_get_current(uint8_t motor);
#endif

/**
 * Read the current in a given motor in milliamps.
 *
 * @param motor Motor to sense current of.
 * @return the current of motor 'motor', in milliamps.
 */
#ifndef SIMULATE
uint16_t motor_get_current_MA(uint8_t motor);
#endif

#endif
