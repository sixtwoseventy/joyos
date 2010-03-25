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
#include <pid.h>

#include <math.h>

#define GYRO_PORT 11

#define MOTOR_LEFT 0
#define MOTOR_RIGHT 4

#define SERVO_BACKRIGHT     0
#define SERVO_BACKLEFT      1
#define SERVO_FRONTRIGHT    2
#define SERVO_FRONTLEFT     3
#define SET_SERVO_PAUSE     100

#define DEGS_TO_RADS        0.01745328f

int display_angle (void) {
    for (;;) {
        int theta = (int) get_degrees ();
        printf("\ntheta = %d", theta);
        pause(500);
    }

    return 0;
}

void setServoStraight () {
    servo_set_pos (SERVO_BACKRIGHT, 202);
    pause (SET_SERVO_PAUSE);
    printf("set backright");
    servo_set_pos (SERVO_BACKLEFT, 280);
    pause (SET_SERVO_PAUSE);
    printf("set backleft");
    servo_set_pos (SERVO_FRONTRIGHT, 222);
    pause (SET_SERVO_PAUSE);
    printf("set frontright");
    servo_set_pos (SERVO_FRONTLEFT, 138);
    pause (SET_SERVO_PAUSE);
    printf("set frontleft");
}

void setServoTurn () {
    servo_set_pos (SERVO_BACKRIGHT, 292);
    pause (SET_SERVO_PAUSE);
    servo_set_pos (SERVO_BACKLEFT, 148);
    pause (SET_SERVO_PAUSE);
    servo_set_pos (SERVO_FRONTRIGHT, 121);
    pause (SET_SERVO_PAUSE);
    servo_set_pos (SERVO_FRONTLEFT, 242);
    pause (SET_SERVO_PAUSE);
}

void setServoSide () {
    servo_set_pos (SERVO_BACKRIGHT, 422);
    pause (SET_SERVO_PAUSE);
    servo_set_pos (SERVO_BACKLEFT, 6);
    pause (SET_SERVO_PAUSE);
    servo_set_pos (SERVO_FRONTRIGHT, 4);
    pause (SET_SERVO_PAUSE);
    servo_set_pos (SERVO_FRONTLEFT, 390);
    pause (SET_SERVO_PAUSE);
}


// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
    //setServoStraight ();

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
    uint16_t dec = (int) floor( value*100 - floor(value)*100 );
    printf("Actuate %d.%03u\n", (int) value, dec);
}

int umain(void) {
    create_thread(&display_angle, 64, 0, "display angle");

    struct pid_controller pid;
    init_pid (&pid, 1, 0, 0, &read_angle, &set_turn);
    pid.goal = 90;
    pid.enabled = 1;

    dispatch_pid (&pid, 0.01, 0);
    printf("pid done.\n");

    return 0;
}
