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

#include "hal/io.h"

void io_init() {
    // XMEM
    MCUCR |= _BV(SRE); //SRAM Enable;
    XMCRB &= ~_BV(XMBK);
    XMCRA &= ~_BV(SRW11);
    MCUCR &= ~_BV(SRW10);

    // Port B: SPI, Beeper, FPGA conf
    DDRB = 0xF7;
    PORTB |= _BV(6);
    // PORT D: I2C, switches, leds
    DDRD = 0xFC;
    // PORTE: uart, lcd, RF
    DDRE = 0x4E;
    PORTE |= _BV(4) | _BV(5);
    // PORTF: ADC, JTAG
    DDRF = 0x0D;
    // PORTG: mem, FPGA conf
    DDRG = 0x00;

    // disable all SPI devices
    SPI_FLASH_SS(1);
    SPI_MOTOR_SS(1);
    SPI_ADC1_SS(1);
    SPI_ADC2_SS(1);
    SPI_RF_SS(1);

    // Stand by mode
    RF_CE(0);
}

#endif
