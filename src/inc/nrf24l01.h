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

#ifndef SIMULATE

#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include <inttypes.h>
// Register Map

// Config Register
#define NRF_REG_CONFIG          0x00

#define NRF_BIT_MASK_RX_DR  6
#define NRF_BIT_MASK_TX_DR  5
#define NRF_BIT_MASK_MAX_RT 4
#define NRF_BIT_EN_CRC          3
#define NRF_BIT_CRCO                2
#define NRF_BIT_PWR_UP          1
#define NRF_BIT_PRIM_RX         0

// Auto Acknowledge
#define NRF_REG_EN_AA               0x01

#define NRF_BIT_ENAA_P5         5
#define NRF_BIT_ENAA_P4         4
#define NRF_BIT_ENAA_P3         3
#define NRF_BIT_ENAA_P2         2
#define NRF_BIT_ENAA_P1         1
#define NRF_BIT_ENAA_P0         0

// Enable RX Addresses
#define NRF_REG_EN_RXADDR       0x02

#define NRF_BIT_ERX_P5          5
#define NRF_BIT_ERX_P4          4
#define NRF_BIT_ERX_P3          3
#define NRF_BIT_ERX_P2          2
#define NRF_BIT_ERX_P1          1
#define NRF_BIT_ERX_P0          0

// Address Width
#define NRF_REG_SETUP_AW        0x03

#define NRF_AW_5                        3
#define NRF_AW_4                        2
#define NRF_AW_3                        1

// Auto Retransmission
#define NRF_REG_SETUP_RETR  0x04

#define NRF_RETR_ARD_BASE       4
#define NRF_RETR_ARC_BASE       0

// RF Channel
#define NRF_REG_RF_CH               0x05

// RF Setup
#define NRF_REG_RF_SETUP        0x06

#define NRF_BIT_PLL_LOCK        4
#define NRF_BIT_RF_DR_BASE      3
#define NRF_RF_DR_1MBPS         0
#define NRF_RF_DR_2MBPS         1
#define NRF_RF_DR_250KBPS       4
#define NRF_RF_PWR_BASE         1
#define NRF_RF_PWR_18DB         0
#define NRF_RF_PWR_12DB         1
#define NRF_RF_PWR_6DB          2
#define NRF_RF_PWR_0DB          3
#define NRF_BIT_LNA_HCURR       0

// Status
#define NRF_REG_STATUS          0x07

#define NRF_BIT_RX_DR               6
#define NRF_BIT_TX_DS               5
#define NRF_BIT_MAX_RT          4
#define NRF_RX_P_NO_BASE        1
#define NRF_RX_P_NO_NONE        6
#define NRF_RX_P_NO_EMPTY       7
#define NRF_RX_P_NO_MASK        0x0E
#define NRF_BIT_TX_FULL         0

// Transmit observe
#define NRF_REG_OBSERVE_TX  0x08

#define NRF_PLOS_CNT_BASE       4
#define NRF_ARC_CNT_BASE        0

// Carrier Detect
#define NRF_REG_RPD             0x09

// Receive Address Pipe 0
#define NRF_REG_RX_ADDR_P0  0x0A

// Receive Address Pipe 1
#define NRF_REG_RX_ADDR_P1  0x0B

// Receive Address Pipe 2
#define NRF_REG_RX_ADDR_P2  0x0C

// Receive Address Pipe 3
#define NRF_REG_RX_ADDR_P3  0x0D

// Receive Address Pipe 4
#define NRF_REG_RX_ADDR_P4  0x0E

// Receive Address Pipe 5
#define NRF_REG_RX_ADDR_P5  0x0F

// Transmit Address
#define NRF_REG_TX_ADDR         0x10

// Payload Width Pipe 0
#define NRF_REG_RX_PW_P0        0x11

// Payload Width Pipe 1
#define NRF_REG_RX_PW_P1        0x12

// Payload Width Pipe 2
#define NRF_REG_RX_PW_P2        0x13

// Payload Width Pipe 3
#define NRF_REG_RX_PW_P3        0x14

// Payload Width Pipe 4
#define NRF_REG_RX_PW_P4        0x15

// Payload Width Pipe 5
#define NRF_REG_RX_PW_P5        0x16

// FIFO Status
#define NRF_REG_FIFO_STATUS 0x17

#define NRF_BIT_FTX_REUSE       6
#define NRF_BIT_FTX_FULL        5
#define NRF_BIT_FTX_EMPTY       4
#define NRF_BIT_FRX_FULL        1
#define NRF_BIT_FRX_EMPTY       0

// Dynamic payload length
#define NRF_REG_DYNPD           0x1C

#define NRF_BIT_DPL_P5          5
#define NRF_BIT_DPL_P4          4
#define NRF_BIT_DPL_P3          3
#define NRF_BIT_DPL_P2          2
#define NRF_BIT_DPL_P1          1
#define NRF_BIT_DPL_P0          0

// Features
#define NRF_REG_FEATURE         0x1D

#define NRF_BIT_EN_DPL          2
#define NRF_BIT_ACK_PAY         1
#define NRF_BIT_DYN_ACK         0

// SPI Commands
#define NRF_SPI_R_REGISTER          0x00
#define NRF_SPI_W_REGISTER          0x20
#define NRF_SPI_R_RX_PAYLOAD        0x61
#define NRF_SPI_W_TX_PAYLOAD        0xA0
#define NRF_SPI_FLUSH_TX            0xE1
#define NRF_SPI_FLUSH_RX            0xE2
#define NRF_SPI_REUSE_TX_PL         0xE3
#define NRF_SPI_R_RX_LP_WID         0x60
#define NRF_SPI_W_ACK_PAYLOAD       0xA8
#define NRF_SPI_W_TX_PAYLOAD_NOACK  0xB0
#define NRF_SPI_NOP                 0xFF

uint8_t nrf_read_status(void);
uint8_t nrf_read_reg(uint8_t reg);
uint8_t nrf_read_multibyte_reg(uint8_t reg, uint8_t *data, uint8_t len);
uint8_t nrf_write_reg(uint8_t reg, uint8_t data);
uint8_t nrf_write_multibyte_reg(uint8_t reg, uint8_t *data, uint8_t len);
uint8_t nrf_read_rx_payload(uint8_t *data, uint8_t len);
uint8_t nrf_read_rx_payload_len();
uint8_t nrf_write_tx_payload(uint8_t *data, uint8_t len);
uint8_t nrf_flush_tx();
uint8_t nrf_flush_rx();
uint8_t nrf_reuse_tx_pl();
uint8_t nrf_get_packet(uint8_t *buf, uint8_t *size);

#endif

#endif
