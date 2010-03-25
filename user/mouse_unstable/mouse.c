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
#include <math.h>
#include <lib/pid.h>

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#define GYRO_PORT       11
#define LSB_US_PER_DEG  1400000

#define MaxVelocity 100
#define TARGET_VELOCITY 60.
#define TURNING_SCALE .8

#define ACTIVATE_THRESH 500
#define DEACTIVATE_THRESH 375
#define DEACTIVATE_SAMPLES 10
#define START_RAMPDOWN 5

#define RINGKP 1/200.
#define RINGKD 0.
#define RINGKI 0.

//PID

//10 2 0 0
#define PID_DELAY 10
#define KP 1.5
#define KD 0
#define KI 0

#define TURN_THRESH 60*KP
#define TURN_VEL 40

uint8_t Compass = 0x42;
uint8_t ReadCommand[] = {'A'};
uint8_t ResetCommand[] = {'O'};
uint8_t OutputAngleCommand[] = {'G',0x4E,0x00};
uint8_t OutputMagXCommand[] = {'G',0x4E,0x03};
uint8_t OutputMagYCommand[] = {'G',0x4E,0x04};
uint8_t Data[2];
uint16_t AngleReading;
int16_t MagXRawReading;
int16_t MagXReading;
int16_t MagYReading;
uint8_t was_overload;
uint32_t old_time;
float TargetVelocity = 0;
float FieldStrength = 0;
uint16_t PIDDelay;
struct pid_controller ForwardPID;
uint8_t isActivated;
uint8_t isForward;
struct pid_controller RingPID;

float NearbyScale = 1.0f;
//.7
#define NEARBY_DAMP .5
#define NEARBY_RAMP 1.2
#define NEARBY_MIN .2

float fangle;
float PeakStrength = ACTIVATE_THRESH;

int usetup (void) {
    /*
       printf("\nPlace robot,    press go.");
       go_click ();
       printf ("\nStabilizing...");
       pause (500);
       printf ("\nCalibrating     offset...\n");
       gyro_init (GYRO_PORT, LSB_US_PER_DEG, 500L);*/
    return 0;
}

void SetTargetVelocity(float vel) {
    TargetVelocity = vel;
    printf("field strength: %f, peak strength: %f, target vel: %f\n", FieldStrength, PeakStrength, vel);
}


float BoundedVelocity(float ProposedVelocity)
{
    if (ProposedVelocity < -MaxVelocity)
    {
        return -MaxVelocity;
    }
    else if (ProposedVelocity > MaxVelocity)
    {
        return MaxVelocity;
    }
    else
    {
        return ProposedVelocity;
    }
}

void SetRightVelocity(float vel)
{
    motor_set_vel(RIGHT_MOTOR, (int16_t)vel);
    //printf("\nR: %d",(int16_t)vel);
}

void SetLeftVelocity(float vel)
{
    motor_set_vel(LEFT_MOTOR, (int16_t)vel);
    //printf("\nL: %d",(int16_t)vel);
}

void ForwardApplyMV(float MV)
{
    if (!isActivated) MV = 0.;


    if (isForward) {
        if (MV > TURN_THRESH) {
            SetRightVelocity(TURN_VEL);
            SetLeftVelocity(-TURN_VEL);
        } else if (MV < -TURN_THRESH) {
            SetRightVelocity(-TURN_VEL);
            SetLeftVelocity(TURN_VEL);
        } else {
            SetRightVelocity(BoundedVelocity(TargetVelocity+MV/2));
            SetLeftVelocity(BoundedVelocity(TargetVelocity-MV/2));
        }
    } else {
        if (MV > TURN_THRESH) {
            SetRightVelocity(TURN_VEL);
            SetLeftVelocity(-TURN_VEL);
        } else if (MV < -TURN_THRESH) {
            SetRightVelocity(-TURN_VEL);
            SetLeftVelocity(TURN_VEL);
        } else {
            SetRightVelocity(BoundedVelocity(-TargetVelocity+MV/2));
            SetLeftVelocity(BoundedVelocity(-TargetVelocity-MV/2));
        }
    }
}

void MotorCoast(uint8_t Motor)
{
    motor_set_vel(Motor, 0);
}

void CoastMotors()
{
    MotorCoast(LEFT_MOTOR);
    MotorCoast(RIGHT_MOTOR);
}

float
strength(int16_t MagX, int16_t MagY){
    float fMagX = (float)(MagX);
    float fMagY = (float)(MagY);

    return sqrt(fMagX*fMagX+fMagY*fMagY);
}

/*int16_t bounded_vel(float vel){
  if (vel > 150.0) return 150;
  else if (vel < 66.6) return 67;
  else return (int16_t)vel;
  }*/

void MagnetInit() {
    old_time = get_time();

    was_overload=0;

    i2cInit();

    i2cMasterSend(Compass,1,ResetCommand);
    pause(10);
}

float ComputeMagnetTarget() {
    /* Reset periodically */

    /*if ((get_time() - old_time) > 250){
            old_time = get_time();
            i2cMasterSend(Compass,1,ResetCommand);
            pause(10);

            }*/
    i2cMasterSend(Compass,1,ResetCommand);
    pause(10);


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

    //printf("\nX:%.1f,%.1f, Y:%.1f,%.1f",MagXReading/10.0,MagXRawReading/10.0,MagYReading/10.0,MagYRawReading/10.0);
    //printf("\n(%.1f,%.1f)",MagXReading/10.0,MagYReading/10.0);

    FieldStrength = strength(MagXReading,MagYReading);

    // If we encounter a large magnetic field, use that as the new peak
    PeakStrength = FieldStrength > PeakStrength ? FieldStrength : PeakStrength;
    /*
       if (FieldStrength > ACTIVATE_THRESH){
       was_overload = 1;
       }
       else if (was_overload && (FieldStrength < DEACTIVATE_THRESH)){
       was_overload = 0;
       old_time = get_time();
       i2cMasterSend(Compass,1,ResetCommand);
       pause(10);
       }
     */
    fangle = ((float)AngleReading)/10.0;
    if (fangle > 180.0) fangle -= 360.0;

    //printf("\nangle:%.1f, strength: %.2f",fangle, strength(MagXReading,MagYReading));


    //i2cMasterSend(Compass,1,ResetCommand);
    //pause(10);

    return fangle;
}

float getError() {
    if (isForward) {
        return -fangle;
    } else {
        return (-(int)fangle+360)%360-180;
    }
}

float getStrengthError() {
    if (!isActivated) return 0.;
    else return (- PeakStrength/2.0 + FieldStrength);
}

void applyNearbyScale(float val) {
    printf("val: %.2f\n",val);
    if (val < 0.){
        NearbyScale = -val;
        isForward = !isForward;
    } else {
        NearbyScale = val;
    }
}

void PIDInit() {
    SetTargetVelocity(TARGET_VELOCITY);

    printf("\n");

    init_pid(&ForwardPID,
            (float)KP,
            (float)KI,
            (float)KD,
            &getError,
            &ForwardApplyMV);
    ForwardPID.goal = 0.0;
    ForwardPID.enabled = 1;

    init_pid(&RingPID,
            RINGKP,
            RINGKD,
            RINGKI,
            &getStrengthError,
            &applyNearbyScale);
    RingPID.goal = 0.0;
    RingPID.enabled = 1;
}

void PIDUpdate() {
}

void RingReset() {
    init_pid(&RingPID,
            RINGKP,
            RINGKD,
            RINGKI,
            &getStrengthError,
            &applyNearbyScale);
    RingPID.goal = 0.0;
    RingPID.enabled = 1;
}
void PIDReset() {
    init_pid(&ForwardPID,
            (float)KP,
            (float)KD,
            (float)KI,
            &getError,
            &ForwardApplyMV);
    ForwardPID.goal = 0.0;
    ForwardPID.enabled = 1;

    RingReset();

}


/**
 * Run all tests
 */
int
umain (void) {


    isActivated = false;
    isForward = true;
    MagnetInit();
    PIDInit();
    uint8_t num_low_readings = 0;

    while(1){

        //Read Magnet data
        ComputeMagnetTarget();

        //uint8_t oldForward = isForward;

        if (isForward && (fangle > 90. || fangle < -90.)) {
            isForward = false;
        } else if (!isForward && (-90. < fangle && fangle < 90.)) {
            isForward = true;
        }

        if (FieldStrength < DEACTIVATE_THRESH) isActivated = false;
        else if (FieldStrength > ACTIVATE_THRESH) isActivated = true;

        update_pid(&RingPID);

        //Determine if we should change from activated->deactivated or vice versa
        SetTargetVelocity(NearbyScale*TARGET_VELOCITY);
        update_pid(&ForwardPID);

        //PIDUpdate();

    }

    return 0;

}

