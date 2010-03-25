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

#ifndef MOTION_H
#define MOTION_H

#include <kern/global.h>
#include <lib/motor_group.h>
#include <lib/pid.h>

/**
 * \file motion.h
 * \brief A PID-controlled MotorGroup
 *
 * A MotionController provides a PID-controlled MotorGroup.
 *
 * Given a MotorGroup (one or more motors), an encoder port and a target
 * position, the MotionController will attempt to drive the MotorGroup to the
 * given target.
 */

/**
 * MotionController structure
 */
typedef struct {
    int32_t position;           ///< current encoder position
    uint8_t encoder;            ///< encoder port to read from
    uint16_t encoder_old_pos;   ///< old encoder position
    MotorGroup output;          ///< MotorGroup to drive
    struct pid_controller pid;  ///< PID controller
} MotionController;

/**
 * Create a new MotionController
 *
 * @param motion        MotionController to initialise
 * @param motor         MotorGroup to drive
 * @param encoder_port  Encoder port to read motor position from
 * @param kp            Proportional constant for PID
 * @param ki            Integral constant for PID
 * @param kd            Derivative constant for PID
 */
void motion_init(MotionController *motion, MotorGroup motor, uint8_t encoder_port,
        float kp, float ki, float kd);

/**
 * Set the target position for the MotionController. The controller will
 * immediately start driving the motor towards the goal.
 *
 * @param motion  MotionController to set
 * @param goal    Goal position to drive towards
 */
void motion_set_goal(MotionController *motion, int32_t goal);

/**
 * Perform one cycle of the motion controller loop. Call as often as
 * possible.
 *
 * @param motion   MotionController to run
 */
void motion_update(MotionController *motion);

/**
 * Check if the MotionController's goal has been reached
 *
 * @returns true if goal reached, false otherwise
 */
bool motion_goal_reached(MotionController *motion);

#endif
