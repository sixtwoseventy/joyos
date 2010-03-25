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

// Include headers from OS
#include <joyos.h>
#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

int16_t left_mot_vel=75;
int16_t right_mot_vel=75;

// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
    return 0;
}

void inc_left(){
    if (left_mot_vel < 80){
        left_mot_vel++;
        motor_set_vel(LEFT_MOTOR,left_mot_vel);
    }
}

void dec_left(){
    if (left_mot_vel > 70){
        left_mot_vel--;
        motor_set_vel(LEFT_MOTOR,left_mot_vel);
    }
}

// Entry point to contestant code.
// Create threads and return 0.

// Move from
// (-733,59) angle: 23
// (-49,-1111) angle: -1021
// (867,-372) angle: -1761
// (136,624) angle: 1171
// (this is CW movement in a circle)
// diameter: 1500, centered at 0

int umain (void) {

    int32_t long_x;
    int32_t long_y;

    //motor_set_vel(LEFT_MOTOR,left_mot_vel);
    //motor_set_vel(RIGHT_MOTOR,right_mot_vel);

    //This loop prints out its position, which is set by RF
    //When go is pressed, a position is sent out, and other happyboards will update their position accordingly
    //Thus, this demo needs two happyboards to witness in action
    while(1)
    {
        //printf("\n(%d,%d) angle: %d",objects[0].x,objects[0].y,objects[0].theta);

        long_x = (int32_t)objects[0].x;
        long_y = (int32_t)objects[0].y;

        if ((long_x*long_x+long_y*long_y) < 750L*750L){
            //dec_left();
            printf("inside.\n");
        }
        else{
            //inc_left();
            printf("outside.\n");
        }

        if (go_press()) {
            //transmit_position_packet(1,get_time()/1000.0,3.14159); //1 is the index of the receiving robot.  For now, this is 1 for all robots
        }
        delay_busy_ms(100);
    }

    // Will never return, but the compiler complains without a return
    // statement.
    return 0;
}
