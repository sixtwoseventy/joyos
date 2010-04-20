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

#ifndef __INCLUDE_BOARD_H__
#define __INCLUDE_BOARD_H__

#ifndef SIMULATE

#include <hal/uart.h>
#include <lcd.h>
#include <fpga.h>
#include <servo.h>
#include <analog.h>
#include <digital.h>
#include <encoder.h>
#include <motor.h>
#include <buttons.h>
#include <rf.h>

#else

#include <joyos.h>

#endif

/**
 * \file board.h
 * \brief Common board functionality.
 *
 * This file initializes most of the drivers for the happyboard.
 */

#define BOARD_CONFIG_ADDRESS 0
#define FPGA_CONFIG_ADDRESS 256

/**
 * Happyboard configuration structure stores basic board config
 */
typedef struct {
    uint16_t version;       ///< Happyboard hardware version
    uint16_t id;            ///< Unique board id
    uint16_t fpga_version;  ///< FPGA firmware version
    uint16_t fpga_len;      ///< FPGA firmware length
    uint16_t crc;           ///< Config CRC
} BoardConfig;

BoardConfig board_config;

/**
 * Initializes the board. Should not be called by the user. It is run before user
 * code near the beginnning of the JoyOS startup sequence. Specifically:
 *
 * 1) It sets various registers in the ATMega128 for IO, eXternal MEMory, and UART.
 *
 * 2) It loads the FPGA code, board config, checks the battery.
 *
 * 3) It initializes many locks, including those for: uart, digital, encoder, SPI,
 *    motor, servo, LCD, ADC, and ISR.
 *
 * 4) It allocates memory for malloc() usage; defaults to having .heap section in
 *    external memory (SRAM chip).
 */
void board_init (void);

#endif // __INCLUDE_BOARD_H__
