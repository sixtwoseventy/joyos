#ifndef _RF_H_
#define _RF_H_

#include <nrf24l01.h>
#include <packet.h>
/**
 * \file rf.h
 * \brief User transmission and receipt of packet information over RF
 *
 * This file allows the user to transmit packets using the nrf24l01 module,
 * as well as retrieve information received over RF
 */ 

extern packet_buffer tx, rx;

extern volatile board_coord objects[4];
extern volatile uint32_t position_microtime;

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

