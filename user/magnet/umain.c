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

#include <joyos.h>

uint8_t Compass = 0x42;
uint8_t ReadCommand[] = {'A'};
uint8_t ResetCommand[] = {'O'};
uint8_t OutputAngleCommand[] = {'G',0x4E,0x00};
uint8_t OutputMagXCommand[] = {'G',0x4E,0x03};
uint8_t OutputMagYCommand[] = {'G',0x4E,0x04};
uint8_t Data[2];
uint16_t AngleReading;
int16_t MagXReading;
int16_t MagYReading;
uint8_t was_overload;
uint32_t old_time;

float fangle;

int usetup (void) {
    return 0;
}

int16_t strength(int16_t MagX, int16_t MagY){
    if (MagX < 0) MagX = -MagX;
    if (MagY < 0) MagY = -MagY;

    return MagX+MagY;
}

int16_t bounded_vel(float vel){
    if (vel > 150.0) return 150;
    else if (vel < 66.6) return 67;
    else return (int16_t)vel;
}

/**
 * Run all tests
 */
int umain (void) {


    old_time = get_time();

    was_overload=0;

    i2cInit();

    i2cMasterSend(Compass,1,ResetCommand);
    pause(10);

    while(1){

        /* Reset periodically */

        /*if ((get_time() - old_time) > 250){
                old_time = get_time();
                i2cMasterSend(Compass,1,ResetCommand);
                pause(10);

                }*/

        /* Read Heading Information */

        i2cMasterSend(Compass,3,OutputAngleCommand);
        i2cMasterSend(Compass,1,ReadCommand);

        pause(10);

        i2cMasterReceive(Compass,2,Data);
        AngleReading = (Data[0] << 8) | Data[1];

        /* Read Offset-Corrected Magnetometer X */

        i2cMasterSend(Compass,3,OutputMagXCommand);
        i2cMasterSend(Compass,1,ReadCommand);

        pause(10);

        i2cMasterReceive(Compass,2,Data);
        MagXReading = (Data[0] << 8) | Data[1];

        /* Read Offset-Corrected Magnetometer X */

        i2cMasterSend(Compass,3,OutputMagYCommand);
        i2cMasterSend(Compass,1,ReadCommand);

        pause(10);

        i2cMasterReceive(Compass,2,Data);
        MagYReading = (Data[0] << 8) | Data[1];

        if (strength(MagXReading,MagYReading) > 500){
            was_overload = 1;
        }
        else if (was_overload & (strength(MagXReading,MagYReading) < 250)){
            was_overload = 0;
            old_time = get_time();
            i2cMasterSend(Compass,1,ResetCommand);
            pause(10);
        }

        printf("\n%d.%d, (%d, %d)",AngleReading/10,AngleReading%10,MagXReading,MagYReading);

        fangle = ((float)AngleReading)/10.0;
        if (fangle > 180.0) fangle -= 360.0;

        motor_set_vel(0,bounded_vel(15.0-fangle));
        motor_set_vel(1,bounded_vel(15.0+fangle));

        pause(100);

    }

    return 0;

}
