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
 * The happyboard LCD is a 2x16 character device. The OS code treats the LCD as
 * a single 1x32 line display. This means that printf("012345690123456789")
 * will display:
 *
 * \code
 *         +----------------+
 * line 1: |0123456789012345|
 * line 2: |6789            |
 *         +----------------+
 * \endcode
 *
 * For general LCD printing, printf() is suggested. printf() is a wrapper around
 * the functions documented below. As such these functions should only be used
 * when printf() is insufficient.
 *
 * Printing a newline character ("\n" in C) to the display clears it. Thus,
 * printf("hello world\n"), will not display anything (the display is cleared
 * after the text is displayed). Instead, you should use printf("\nhello world")
 * - this will clear the display and then display the text.
 *
 * The OS will occasionally use the bottom right character of the display for
 * warning/information icons, which may obscure user printouts (if they use the
 * full 32 characters of the display).
 *
 * The LCD supports 8 user-defined characters. On startup these characters are
 * set to a system defaults as follows:
 *
 * \code
 * 0: unsused
 * 1: smiley face
 * 2: alternate smiley face
 * 3: frowny face
 * 4: low battery indicator
 * 5: loading indicator
 * 6: bootloader indicator
 * 7: unused
 * \endcode
 *
 * These special characters can be reconfigured by the user with the
 * lcd_set_custom_char() function, as long as you remember this will change the
 * OS warning indicator icons.
 *
 */

#ifndef SIMULATE

#include <inttypes.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#else

#include <joyos.h>

#endif

#ifndef SIMULATE

// TODO lock

/**
 * Initialise the LCD driver
 */
void lcd_init(void);

/**
 * Sets a custom LCD character. The LCD has space for 8 custom characters (characters 0-7.) Note that the OS uses some of these charaters for the status indicator. The data is read from program (flash) memory and therefore should be defined as PROGMEM, eg:
 * \code
 * uint8_t my_custom_char[] PROGMEM = { 12, 23, 34, 45, 56, 67, 78, 89 };
 * \endcode
 *
 * @param chnum     character to set
 * @param data      8 byte array of pixel data
 */
void lcd_set_custom_char(uint8_t chnum,uint8_t *data);

/**
 * Write a data byte to the LCD. Should not normally be needed by user code.
 *
 * @param data      byte to write.
 */
void lcd_write_data(uint8_t data);

/**
 * Display a string to the LCD.
 *
 * @param string    string to display
 */
void lcd_print(const char *string);

/**
 * Display a formated string to the LCD.
 *
 * @param fmt       format to display
 * @param ap        virtual argument list
 */
int lcd_vprintf(const char *fmt, va_list ap);

#endif

/**
 * Display a formated string to the LCD.
 *
 * @param fmt       format to display
 */
int lcd_printf(const char *fmt, ...);

#ifndef SIMULATE

int lcd_vprintf_P (PGM_P fmt, va_list ap);
int lcd_printf_P (PGM_P fmt, ...);

/**
 * Print a single character to the LCD.
 *
 * @param ch        character to display
 * @param f         ignored
 */
int lcd_print_char(char ch, FILE *f);

/**
 * Clear the LCD Screen.
 */
void lcd_clear(void);

/**
 * Return the current cursor position.
 */
uint8_t lcd_get_pos(void);

/**
 * Set the current cursor position.
 *
 * @param p     position to move to [0..31]
 */
void lcd_set_pos(uint8_t p);

#endif

#endif
