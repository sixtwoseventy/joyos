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

#include <joyos.h>

#define GYRO_PORT       11
#define LSB_US_PER_DEG  1400000

uint8_t team_number={1,0};

// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
    printf("\nPlace robot,    press go.");
    go_click ();
    printf ("\nStabilizing...");
    pause (500);
    printf ("\nCalibrating     offset...\n");
    gyro_init (GYRO_PORT, LSB_US_PER_DEG, 500L);
    return 0;
}

int umain(void) {
    for (;;) {
        printf ("\ntheta = %.2f", gyro_get_degrees());
        pause (100);
    }
    return 0;
}
