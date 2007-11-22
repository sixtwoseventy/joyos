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

#ifndef _LCD_H_
#define _LCD_H_

/**
 * \file lcd.h
 * \brief LCD control, formatting, printout.
 *
 * The happyboard LCD is a 2x16 character device. The OS code treats the LCD as a single 
 * 1x32 line display. This means that printf("012345690123456789") will display:
 *
 * line 1: "0123456789012345"<br>
 * line 2: "6789"
 *
 * Printing a newline character to the display clears it. Thus, printf("hello world\n"), 
 * will not display anything (the display is cleared after the text is displayed). Instead,
 * you should use printf("\nhello world").
 */ 

#include <inttypes.h>
#include <stdio.h>

// TODO lock

/**
 * Initialise the LCD driver
 */
void lcdInit(void);

/**
 * Sets a custom LCD character. The LCD has space for 8 custom characters (characters 0-7.) Note that the OS uses some of these charaters for the status indicator
 * @param chnum		character to set
 * @param data		8 byte pixel data
 */
void lcdSetCustomChar(uint8_t chnum,uint8_t *data);

/**
 * Write a data byte to the LCD. Should not normally be needed by user code.
 * @param data		byte to write.
 */
void lcdWriteData(uint8_t data);

/**
 * Display a string to the LCD.
 * @param string	string to display
 */
void lcdPrint(const char *string);

/**
 * Display a formated string to the LCD.
 * @param format	format to display
 * @param ap		virtual argument list
 */
int lcd_vprintf(const char *fmt, va_list ap);

/**
 * Display a formated string to the LCD.
 * @param format	format to display
 */
int lcd_printf(const char *fmt, ...);

/**
 * Print a single character to the LCD.
 * @param ch		character to display
 */
int lcdPrintChar(char ch, FILE *f);

/**
 * Clear the LCD Screen.
 */
void lcdClear(void);

/**
 * Return the current cursor position.
 */
uint8_t lcdGetPos(void);

/**
 * Set the current cursor position.
 */
void lcdSetPos(uint8_t p);

#endif
