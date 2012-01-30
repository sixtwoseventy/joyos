#ifndef _RF_H_
#define _RF_H_

#include <nrf24l01.h>
#include <packet.h>
#include <kern/pipe.h>
/**
 * \file rf.h
 * \brief User transmission and receipt of packet information over RF
 *
 * This file allows the user to transmit packets using the nrf24l01 module,
 * as well as retrieve information received over RF
 */

extern volatile game_data game;
extern volatile uint32_t position_microtime;

#ifndef SIMULATE

extern volatile board_coord goal_position; //The target position received from a goal packet

/**
 * Initializes RF.  Should not be called by user
 */
void rf_init();

void rf_poll(pipe *p);
int rf_fflush(pipe *p);

void copy_objects();

extern volatile uint8_t robot_id;

#endif

#endif //_RF_H_

