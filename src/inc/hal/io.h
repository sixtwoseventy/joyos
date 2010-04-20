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

#ifndef _IO_H_
#define _IO_H_

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

/**
 * \file io.h
 * \brief General Purpose IO support
 */

#define GPIO_WRITE(val, port, pin) do { \
    if (val) (port)|=(uint8_t)_BV(pin); \
    else (port)&=(uint8_t)~((uint8_t)_BV(pin)); \
} while (0)

#define GPIO_WRITE_PORT(val, port, mask) do { \
    (port)&=~(mask); \
    (port)|=(val); \
} while (0)

#define GPIO_READ(port, pin) (((uint8_t)(port) & (uint8_t)_BV(pin))!=0)

#define SPI_ADC1_SS(v)  GPIO_WRITE(v, PORTB, 0)
#define SPI_MOTOR_SS(v) GPIO_WRITE(v, PORTE, 2)
#define SPI_RF_SS(v)    GPIO_WRITE(v, PORTE, 3)
#define SPI_ADC2_SS(v)  GPIO_WRITE(v, PORTF, 0)
#define SPI_FLASH_SS(v) GPIO_WRITE(v, PORTF, 2)

#define RF_CE(v)        GPIO_WRITE(v, PORTF, 3)

#define FPGA_DIN(v)     GPIO_WRITE(v, PORTB, 5)
#define FPGA_PROGB(v)   GPIO_WRITE(v, PORTB, 6)
#define FPGA_CCLK(v)    GPIO_WRITE(v, PORTB, 7)
#define FPGA_DONE()     GPIO_READ(PING, 3)
#define FPGA_INITB()    GPIO_READ(PING, 4)

#define SWITCH_GO()     (!GPIO_READ(PINE, 5))
#define SWITCH_STOP()   (!GPIO_READ(PINE, 4))

#define LED_COMM(v)     GPIO_WRITE(v, PORTE, 6)
//#define LED_PWR(v)    GPIO_WRITE(v, PORTE, 7) #Power light line taken by RF interrupt

#define BEEPER(v)       GPIO_WRITE(v, PORTB, 4)

#define LCD_PIN_RS      7
#define LCD_PIN_E       6
#define LCD_PORT        PORTD

#define LCD_E(v)        GPIO_WRITE(v, LCD_PORT, LCD_PIN_E)
#define LCD_RS(v)       GPIO_WRITE(v, LCD_PORT, LCD_PIN_RS)


/** Initialize basic IO. Should not be called by user. */
void io_init();

#endif

#endif

