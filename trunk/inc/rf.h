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

#ifndef _RF_H_
#define _RF_H_

#include <inttypes.h>

/**
 * \file rf.h
 * \brief RF receiver interface
 *
 * The happyboard includes a 2.4Ghz RF receiver chip which is used to provide 
 * game information to robots during a round. The RF system stores the position
 * of the two robots competing in the current round. This information can be
 * accessed with the rf_get_x() and rf_get_y() functions. The value returned is in
 * 100ths of inches. These values are updated approximately every second, and so
 * should only be used for strategy/oponent location.
 *
 */

#define PKT_ROBOT0		1
#define PKT_ROBOT1		2
#define PKT_START			3

/// The round is currently stopped
#define STATE_STOPPED 0
/// The round is currently running
#define STATE_RUNNING 1

/**
 * Initialise RF System
 */
void rf_init();

/**
 * Set the team number 
 * RF messages are sent to the teams currently competing in a round. 
 * As such the RF code needs to know the team number, so it can receive
 * the correct messages.
 */
void rf_set_team(uint8_t num);

/**
 * Get the X position of the specified robot (0 or 1)
 * returns a value in the range -300 to 300
 */
int16_t rf_get_x(uint8_t robot);

/**
 * Get the Y position of the specified robot (0 or 1)
 * returns a value in the range -400 to 400
 */
int16_t rf_get_y(uint8_t robot);

/**
 * Get the state of the current contest round
 */
uint8_t rf_get_round_state();

/**
 * RF read thread. Should not be called by user.
 */
int rfread (void);

#endif
