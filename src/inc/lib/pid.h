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

#ifndef __INCLUDE_PID_H__
#define __INCLUDE_PID_H__

#include <kern/global.h>

/**
 * \file pid.h
 * \brief Generic PID (Proportional Integral Derivative) controller
 * implementation.
 *
 * A PID is a commonly used feedback mechanism used to correct error
 * between a measured process variable and a desired setpoint.
 *
 * A Common use in robotics (especially in 6.270) is the position control
 * of a motor. In this case the process variable is the wheels angular
 * position, and the setpoint is a desired target position (eg: 720 degrees,
 * or two wheel rotations forward). The wheel position is measured with shaft
 * encoders and the motor is commanded by the PID controller to compensate
 * for the error between the current wheel position and the desired goal.
 *
 * This allows the user to command a desired amount of wheel rotation (and
 * hence distance travelled). With a PID controller on each wheel the user
 * can perform reasonably accurate motions (straight lines, in place turns,
 * etc).
 *
 *
 * For more information see http://en.wikipedia.org/wiki/PID_controller
 */

/**
 * PID Controller structure
 */
struct pid_controller {
    // Constants
    float kp;                     ///< Proportional constant
    float ki;                     ///< Integral constant
    float kd;                     ///< Derivative constant

    // input and output
    float (*input) ();            ///< Input function
    void (*output) (float value); ///< Output drive function

    // goal
    float goal;                   ///< PID target

    // feedback memory
    float sum;                    ///< Integration summation
    uint8_t has_past;             ///< If false, last_val/time are undefined
    float last_val;               ///< previous value for D
    uint32_t last_time;           ///< previous reading time

    // state
    uint8_t enabled; ///< whether the PID is currently running
};

/**
 * Initialize a PID controller structure. This function takes an empty
 * pid_controller structure and initializes its member variables.
 *
 * @param pid       empty PID to initialize
 * @param kp        proportional constant
 * @param ki        integral constant
 * @param kd        derivative constant
 * @param input     input function
 * @param output    output function
 * */
void init_pid (struct pid_controller *pid,
        float kp, float ki, float kd,
        float (*input) (), void (*output) (float value));

/**
 * Perform a single step of the PID loop.
 * like update_pid() except takes a current value as an argument
 *
 * @param pid          PID controller
 * @param current_val  Current control value
 */
float update_pid_input(struct pid_controller *pid, float current_val);

/**
 * Perform a single step of the PID loop.
 * The update_pid() steps are as follows:
 *
 * - check the pid is enabled
 * - call pid->input() and store the result in current
 * - calculate error (pid->goal - current)
 * - calculate P, I, D components and sum result
 * - call pid->output() with result
 *
 * @param pid       PID controller
 */
float update_pid (struct pid_controller *pid);

/**
 * Dispatch to a routine that will drive the PID system until the output
 * equals the goal within tolerance. This routine blocks and only returns
 * when done or 'stop' is non-zero and returns non-zero.
 *
 * @param pid       PID controller
 * @param tolerance how close to get towards the goal
 * @param stop      return when stop() is true
 */
uint8_t dispatch_pid (
        struct pid_controller *pid, float tolerance, uint8_t (*stop) ());

#endif // __INCLUDE_PID_H__

