#ifndef _RF_H_
#define _RF_H_

#include <stdint.h>

/**
 * \file rf.h
 * \brief User transmission and receipt of packet information over RF
 *
 * This file allows the user to transmit packets using the nrf24l01 module,
 * as well as retrieve information received over RF
 */ 

uint8_t master_id; //ID of the master (vision system)
uint8_t self_id; //ID of this robot
uint8_t other_id; //ID of the other robot
uint8_t mouse_id; //ID of the mouse

float self_position[2];
float other_position[2];
float mouse_position[2];

//The ID of the current goal position.  Starts at 0, increments to 1 on first goal
//packet received, and increments whenever a new goal position is received
uint8_t goal_id;

float goal_position[2]; //The target position received from a goal packet

/**
 * Transmits the position of the object with the given ID
 */
void transmit_position_packet(uint8_t id, float x, float y);

/**
 * Transmits the goal position for the object with the given ID
 */
void transmit_goal_packet(uint8_t id, float x, float y);

/**
 * Tells the listed robots to start
 */
void transmit_start_packet(uint8_t num_bots, uint8_t bot_ids[]);

/**
 * Tells the listed robots to stop
 */
void transmit_stop_packet(uint8_t num_bots, uint8_t bot_ids[]);

/**
 * Initializes RF.  Should not be called by user
 */
void rf_init();

/**
 * Gets the X position of this robot
 */
float get_self_position_x();

/**
 * Gets the Y position of this robot
 */
float get_self_position_y();

/**
 * Gets the X position of opponent robot
 */
float get_other_position_x();

/**
 * Gets the Y position of opponent robot
 */
float get_other_position_y();

/**
 * Gets the X position of the mouse
 */
float get_mouse_position_x();

/**
 * Gets the Y position of the mouse
 */
float get_mouse_position_y();

#endif //_RF_H_