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

/* Gyro demo program for gyro with noise injection.
   Howard Samuels, 11/16/2004
   howard.samuels@analog.com

   Modified for Happyboard
   Ross Glashan, 07/05/2005
   rng@mit.edu
 */

#include <global.h>
#include <board.h>
#include <thread.h>
#include <math.h>
#include <gyro.h>
#include <board.h>
//#include <pid.h>

#define GYRO_PORT 11

#define MOTOR_LEFT 4
#define MOTOR_RIGHT 3

#define DEGS_TO_RADS    0.01745328f

int display_angle (void) {
    for (;;) {
        int theta = (int) get_degrees ();
        printf("\ntheta = %d", theta);
        pause(500);
    }

    return 0;
}

// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
    printf("\nPlace robot,    press go.");
    goClick();
    printf("\nStabilizing...");
    pause(500); /* Wait for robot to stabilize mechanically */
    printf("\nCalibrating     offset...");
    init_gyro(GYRO_PORT, 638, 10000);
    printf("\nDone calibration");

    rf_set_team(60);
    return 0;
}

float read_angle () {
    return get_degrees () * DEGS_TO_RADS;
}

void set_turn (float value) {
    printf("\nActuate %f", (double) value);
}

int umain(void) {
    create_thread(&display_angle, 64, 0, "display angle");

    //struct pid_controller pid;
    //init_pid (&pid, 1, 0, 0, &read_angle, &set_turn);

    while (1);

    return 0;
}
