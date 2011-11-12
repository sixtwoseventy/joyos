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

#ifndef _FPGA_H_
#define _FPGA_H_

/**
 * \file fpga.h
 * \brief Low-level FPGA interface
 *
 * These functions are used to communicate with the fgpa, for programming, etc.
 * None of these should ever have to be called by the user.
 */

// FPGA Base Address
#define FPGA_BASE 0x1100

// FPGA Motor Registers
#define FPGA_MOTOR_BASE     0x00
#define FPGA_MOTOR_SIZE     0x02
#define FPGA_MOTOR_CTL      0x00
#define FPGA_MOTOR_VEL      0x01

// FPGA Encoder Registers
#define FPGA_ENCODER_BASE   0x0C
#define FPGA_ENCODER_SIZE   0x02
#define FPGA_ENCODER_LO     0x00
#define FPGA_ENCODER_HI     0x01

// FPGA Digital Register
#define FPGA_DIGITAL_BASE   0x1E
#define FPGA_DIGITAL_PWM_BASE   0x31
#define FPGA_DIGITAL_SIZE   0x01
#define FPGA_DIGITAL_PINMODE 0x30

// FPGA Servo Registers
#define FPGA_SERVO_BASE     0x20
#define FPGA_SERVO_SIZE     0x02
#define FPGA_SERVO_LO       0x00
#define FPGA_SERVO_HI       0x01

// FPGA Version Registers
#define FPGA_VERSION_MAJ    0xFE
#define FPGA_VERSION_MIN    0xFF

/// FPGA Write Byte - write a byte to an FPGA register. Not called by the user.
#define fpga_write_byte(addr, v) ((volatile uint8_t*) FPGA_BASE)[(addr)] = (v)
/// FPGA Read Byte - read a byte from an FPGA register. Not called by the user.
#define fpga_read_byte(addr) ((volatile uint8_t*) FPGA_BASE)[(addr)]
/// Get the FPGA Major Version. Not called by the user.
#define fpga_get_version_major() fpga_read_byte(FPGA_VERSION_MAJ)
/// Get the FPGA Minor Version. Not called by the user.
#define fpga_get_version_minor() fpga_read_byte(FPGA_VERSION_MIN)
/// Initialise FPGA. Not called by the user.
uint8_t fpga_init(uint16_t start, uint16_t len);

#endif

#endif 

