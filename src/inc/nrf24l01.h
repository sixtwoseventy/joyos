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

#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#define RF_INTERRUPT SIG_INTERRUPT7

//Initializes ATMega168 pins
void init_24L01_pins(void);

//Set up nRF24L01 as a transmitter, does not actually send the data,
//(need to call tx_data_nRF24L01() for that)
uint8_t config_tx_nRF24L01(void);

//Sends command to nRF
uint8_t send_byte(uint8_t cmd);

//Sends command to nRF, returns status byte
uint8_t send_command(uint8_t cmd, uint8_t data);

//Sends a number of bytes of payload
void tx_send_payload(uint8_t cmd, uint8_t bytes);

//This sends out the data stored in the rf_tx_array
//data_array must be setup before calling this function
void tx_data_nRF24L01(void);

//Basic SPI to nRF
uint8_t tx_spi_byte(uint8_t outgoing);

//Configures nRF24L01 for recieve mode
void config_rx_nRF24L01(void);

//Gets data from 24L01 and puts it in rf_rx_array, resets all ints
void rx_data_nRF24L01(void);

#endif
