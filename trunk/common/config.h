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

#ifndef _CONFIG_H
#define _CONFIG_H

#undef XRAMEND
#define XRAMEND 0x8400

#define F_CPU 8000000UL

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>

#define DIGITAL_OUT(val, port, pin) do { if (val) (port)|=(uint8_t)_BV(pin); else (port)&=(uint8_t)~((uint8_t)_BV(pin)); } while (0)
#define DIGITAL_IN(port, pin) (((uint8_t)(port) & (uint8_t)_BV(pin))!=0)

#define SPI_FLASH_SS(v)	DIGITAL_OUT(v, PORTF, 2)
#define SPI_MOTOR_SS(v)	DIGITAL_OUT(v, PORTD, 2)
#define SPI_ADC_SS(v)		DIGITAL_OUT(v, PORTB, 0)
#define SPI_RF_SS(v)		DIGITAL_OUT(v, PORTD, 3)

#define FPGA_DIN(v) 		DIGITAL_OUT(v, PORTB, 5)
#define FPGA_PROGB(v) 	DIGITAL_OUT(v, PORTB, 6)
#define FPGA_CCLK(v) 		DIGITAL_OUT(v, PORTB, 7)
#define FPGA_DONE()			DIGITAL_IN(PING, 3)
#define FPGA_INITB()		DIGITAL_IN(PING, 4)

#define SWITCH_GO()			(!DIGITAL_IN(PIND, 4))
#define SWITCH_STOP() 	(!DIGITAL_IN(PIND, 5))

#define LED_COMM(v)			DIGITAL_OUT(v, PORTD, 6)
#define LED_PWR(v)			DIGITAL_OUT(v, PORTD, 7)

#define BEEPER(v)				DIGITAL_OUT(v, PORTB, 4)

#define disable_interrupts(f)	do { *(f) = SREG; cli(); } while (0)
#define enable_interrupts(f)	SREG = (f)

#define BAUD_RATE				19200

#endif


/*
+-+-------------------------------------+
|#|NAME  DIR  PU	USE                   |
+-+-------------------------------------+
|8|PA0		-	 			AD0                   | 
| |PA1		-				AD1                   |
| |PA2		-				AD2                   |
| |PA3		-				AD3                   |
| |PA4		-				AD4                   |
| |PA5		-				AD5                   |
| |PA6		-				AD6                   |
| |PA7		-				AD7                   |
+-+-------------------------------------+
| |                                     |
+-+-------------------------------------+
|8|PB0		1				ADC_CS                |
| |PB1		1				SCK                   |
| |PB2		1				MOSI                  |
| |PB3		0				MISO                  |
| |PB4		1				BEEPER                |
| |PB5		1				FPGA_DATAO            |
| |PB6		1				FPGA_PROGB            |
| |PB7		1				FPGA_CCLK             |
+-+-------------------------------------+
| | DDRB=0xF1                           |
+-+-------------------------------------+
|8|PC0		-				A0                    |
| |PC1		-				A1                    |
| |PC2		-				A2                    |
| |PC3		-				A3                    |
| |PC4		-				A4                    |
| |PC5		-				A5                    |
| |PC6		-				A6                    |
| |PC7		-				A7                    |
+-+-------------------------------------+
| |                                     |
+-+-------------------------------------+
|8|PD0		0				SCL                   |
| |PD1		0				SDA                   |
| |PD2		1		0		MOT_CS                |
| |PD3		1		0		RF_CS                 |
| |PD4		0		1		SWITCH1               |
| |PD5		0		1		SWITCH0               |
| |PD6		1		0		LED0                  |
| |PD7		1		0		LED1                  |
+-+-------------------------------------+
| | DDRD=0xCC, PORTD=0x30               |
+-+-------------------------------------+
|8|PE0		0				RXD                   |
| |PE1		1				TXD                   |
| |PE2		1		0		LCD_E                 |
| |PE3		1		0		LCD_RS                |
| |PE4		1		0		LCD_DATA0             |
| |PE5		1		0		LCD_DATA1             |
| |PE6		1		0		LCD_DATA2             |
| |PE7		1		0		LCD_DATA3             |
+-+-------------------------------------+
| | DDRE=0xFE                           |
+-+-------------------------------------+
|8|PF0		0				ADC_BAT               |
| |PF1		0				ADC_FROB              |
| |PF2		1				FLASH_CS              |
| |PF3		0		0		-                     |
| |PF4		0		0		JTAG                  |
| |PF5		0		0		JTAG                  |
| |PF6		0		0		JTAG                  |
| |PF7		0		0		JTAG                  |
+-+-------------------------------------+
| | DDRF=0x04                           |
+-+-------------------------------------+
|5|PG0		0		0		WR\                   |
| |PG1		0		0		RD\                   |
| |PG2		0		0		ALE                   |
| |PG3		0		0		FPGA_DONE             |
| |PG4		0		0		FPGA_INITB            |
+-+-------------------------------------+
| | DDRG=0x00                           |
+-+-------------------------------------+

*/
