#ifndef _RF_H_
#define _RF_H_

#include <nrf24l01.h>
#include <stdint.h>
#include <stdio.h>

/**
 * \file rf.h
 * \brief User transmission and receipt of packet information over RF
 *
 * This file allows the user to transmit packets using the nrf24l01 module,
 * as well as retrieve information received over RF
 */ 

#define PAYLOAD_SIZE 30

#define POSITION 0x00 //Updated position of this bot, other bot, or mouse
#define START 0x01 //Start of the round
#define STOP 0x02 //End of the round
#define STRING 0x03 //String from bot to board
#define SYNC 0x04 //Not sure how to use yet.  Established IDs of all bots/vision system on board
#define GOAL 0x05 //Sets the target position of this robot

typedef struct {
	uint8_t id;
	signed x : 12;
	signed y : 12;
	signed theta : 12;
	unsigned confidence : 12;
} __attribute__ ((aligned (1))) __attribute__ ((packed)) board_coord;

typedef struct{
	uint8_t type;
	uint8_t address;
	uint8_t payload[PAYLOAD_SIZE];
} packet;

typedef struct {
	uint8_t type;
	uint8_t address;
	union {
		uint8_t array[PAYLOAD_SIZE];
		board_coord coords[4];
	} payload;
} __attribute__ ((packed)) packet_buffer;

extern packet_buffer tx, rx;

extern volatile board_coord objects[4];

extern board_coord goal_position; //The target position received from a goal packet

extern volatile char rf_str_buf[PAYLOAD_SIZE+1];

// rf_new_str is set by the rx interrupt handler and
// cleared by user code once the string has been
// processed as desired.
extern volatile uint8_t rf_new_str;

int rf_send(char ch);
int rf_put(char ch, FILE *f);
int rf_vprintf(const char *fmt, va_list ap);
int rf_printf(const char *fmt, ...);
int rf_vprintf_P(const char *fmt, va_list ap);
int rf_printf_P(const char *fmt, ...);


/**
 * Prototypes for rf_scanf()
 */
int rf_get(FILE * foo);
char rf_recv();
int rf_vscanf(const char *fmt, va_list ap);
int rf_scanf(const char *fmt, ...);
int rf_vscanf_P(const char *fmt, va_list ap);
int rf_scanf_P(const char *fmt, ...);
uint8_t rf_has_char();

/**
 * Transmits a packet
 */
uint8_t rf_send_packet(uint8_t address, uint8_t *data, uint8_t len);

/**
 * Initializes RF.  Should not be called by user
 */
void rf_init();

#endif //_RF_H_

