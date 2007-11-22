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

#include "lcd.h"
#include "config.h"
#include <avr/pgmspace.h>
#include <lock.h>
#include <thread.h>

#define LCD_PIN_RS		3
#define LCD_PIN_E			2
#define LCD_PORT			PORTE

// LCD command set

// cmd 0x01 : clear display
#define LCD_CLR				0x01

// cmd 0x02 : cursor home
#define LCD_HOME			0x02

// cmd 0x04 : entry mode
#define LCD_ENTRYMODE	0x04
#define LCD_CURSINC		2
#define LCD_DISPSHIFT	1

// cmd 0x08 : display enable
#define LCD_DISPCTL		0x08
#define LCD_DISPON		4
#define LCD_CURSON		2
#define LCD_CURSBLK		1

// cmd 0x10 : move cursor
#define LCD_MOVE			0x10
#define LCD_DISPSHFT	8
#define LCD_DISPDIR		4

// cmd 0x20 : set interface length
#define LCD_IFLEN			0x20
#define LCD_8BIT			16
#define LCD_2LINES		8
#define LCD_FNT5X10		4

// cmd 0x40 : set cgram address
#define LCD_CGADDR		0x40

// cmd 0x80 : set ddram address
#define LCD_DDADDR		0x80

#define lcdWait()			_delay_loop_2(1800)
//#define lcdWait()			pause(2)
#define lcdSetE(v)		DIGITAL_OUT(v, LCD_PORT, LCD_PIN_E)
#define lcdSetRS(v)		DIGITAL_OUT(v, LCD_PORT, LCD_PIN_RS)

unsigned char smileyData0[] PROGMEM	= { 0x00, 0x00, 0x0A, 0x00, 0x00, 0x11, 0x0E, 0x00};	// normal smiley
unsigned char smileyData1[] PROGMEM	= { 0x00, 0x00, 0x0A, 0x00, 0x00, 0x11, 0x1F, 0x00};	// square smiley
unsigned char frownData[] PROGMEM 	= { 0x00, 0x00, 0x0A, 0x00, 0x00, 0x0E, 0x11, 0x00};	// frowny
unsigned char battData[] PROGMEM		= { 0x00, 0x0E, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x00};	// battery low
unsigned char waitData[] PROGMEM 		= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00};	// wait ...
unsigned char dlData[] PROGMEM			= { 0x00, 0x0E, 0x0E, 0x0E, 0x1F, 0x0E, 0x04, 0x00};	// bootloader

uint8_t lcdPos = 0;
uint8_t lcdClearFlag = 0;

// setup LCD file descriptor (for printf)
// NOTE: when printing directly to lcdout, lcd_lock should be held
FILE lcdout = FDEV_SETUP_STREAM(lcdPrintChar, NULL, _FDEV_SETUP_WRITE);
// Lock that protects writing to the lcd
struct lock lcd_lock;

void lcdWriteData(uint8_t data) {
	acquire(&lcd_lock);
	// set data
	lcdSetRS(1);
	
	// write high nibble
	lcdSetE(1);
	PORTE = (data&0xF0) | _BV(LCD_PIN_RS) | _BV(LCD_PIN_E);
	lcdWait();
	lcdSetE(0);
	lcdWait();
	// write low nibble
	lcdSetE(1);
	PORTE = ((data&0x0F)<<4) | _BV(LCD_PIN_RS) | _BV(LCD_PIN_E);
	lcdWait();
	lcdSetE(0);
	lcdWait();
	release(&lcd_lock);
}

void lcdWriteControl(uint8_t data) {
	acquire(&lcd_lock);

	// set control
	lcdSetRS(0);
	
	// write high nibble
	lcdSetE(1);
	PORTE = (data&0xF0) | _BV(LCD_PIN_E);
	lcdWait();
	lcdSetE(0);
	lcdWait();
	// write low nibble
	lcdSetE(1);
	PORTE = ((data&0x0F)<<4) | _BV(LCD_PIN_E);
	lcdWait();
	lcdSetE(0);
	lcdWait();
	release(&lcd_lock);
}

void lcdSetCustomChar(uint8_t chnum,uint8_t *data) {
	acquire(&lcd_lock);
	uint8_t i;
	chnum <<= 3;
	for(i=0; i<8; i++)	{
		lcdWriteControl(LCD_CGADDR|(chnum+i));
		lcdWriteData(pgm_read_byte(data+i));
	}
	release(&lcd_lock);
}

void lcdInit(void) {
	_delay_loop_2(32000);
	lcdWriteControl(0x33); // should be? LCD_IFLEN|LCD_2LINES
	lcdWriteControl(0x32); // should be? LCD_IFLEN|LCD_2LINES
	lcdSetCustomChar(1,(uint8_t*)smileyData0);
	lcdSetCustomChar(2,(uint8_t*)smileyData1);
	lcdSetCustomChar(3,(uint8_t*)frownData);
	lcdSetCustomChar(4,(uint8_t*)battData);
	lcdSetCustomChar(5,(uint8_t*)waitData);
	lcdSetCustomChar(6,(uint8_t*)dlData);
	lcdWriteControl(LCD_CLR);
	lcdWriteControl(LCD_HOME);
	lcdWriteControl(LCD_ENTRYMODE|LCD_CURSINC);
	lcdWriteControl(LCD_DISPCTL|LCD_DISPON);

	init_lock(&lcd_lock, "LCD lock");
}

void lcdPrint(const char *string) {
	acquire(&lcd_lock);
	uint8_t i=0;
	while (*string) {
		if (*string=='\n') {
			lcdWriteControl(LCD_DDADDR);
			lcdPos=0;
			string++;
		}
		else
			lcdWriteData(*string++);
		if (i==15)
			lcdWriteControl(LCD_DDADDR|0x40);
		i++;
		lcdPos++;
	}
	release(&lcd_lock);
}

int
lcd_vprintf(const char *fmt, va_list ap) {
	int count;

	acquire(&lcd_lock);
	count = vfprintf (&lcdout, fmt, ap);
	release(&lcd_lock);

	return count;
}

int
lcd_printf(const char *fmt, ...) {
	va_list ap;
	int count;

	va_start(ap, fmt);
	count = lcd_vprintf(fmt, ap);
	va_end(ap);

	return count;
}

int lcdPrintChar(char ch, FILE *f) {
	// wait for control
	// note: we want to wrap the whole printf in the lcd_lock as well, so
	// the text isn't interleaved... however we acquire here to yield while
	// waiting
	acquire(&lcd_lock);

	if (lcdClearFlag) {
		lcdClear();
		lcdSetPos(0);
		lcdClearFlag = 0;
	}
	if (ch=='\n') {
		lcdClearFlag = 1;
		lcdPos = 0;
	} else {
		lcdWriteData(ch);
		if (lcdPos==15)
			lcdWriteControl(LCD_DDADDR|0x40);
		lcdPos++;
	}

	// give up control
	release(&lcd_lock);
	return ch;
}

// recommended that caller holds lcd_lock
uint8_t lcdGetPos(void) {
	return lcdPos;
}

void lcdSetPos(uint8_t p) {
	acquire(&lcd_lock);
	lcdPos = p;
	if (p<16)
		lcdWriteControl(LCD_DDADDR|p);
	else
		lcdWriteControl(LCD_DDADDR|(p+0x30));	
	release(&lcd_lock);
}

void lcdClear(void) {
	acquire(&lcd_lock);
	lcdWriteControl(LCD_CLR);
	lcdWriteControl(LCD_HOME);
	lcdPos = 0;
	release(&lcd_lock);
}

