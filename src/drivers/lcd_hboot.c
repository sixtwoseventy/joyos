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

#include <lcd_hboot.h>
#include <config.h>
#include <hal/io.h>
#include <hal/delay.h>
#include <avr/pgmspace.h>

#define NIBBLE_HI(b) ((b)>>4)
#define NIBBLE_LO(b) ((b)&0x0F)

// LCD command set
#define LCD_CTRL            0
#define LCD_DATA            1

// cmd 0x01 : clear display
#define LCD_CLR             0x01

// cmd 0x02 : cursor home
#define LCD_HOME            0x02

// cmd 0x04 : entry mode
#define LCD_ENTRYMODE       0x04
#define LCD_CURSINC         2
#define LCD_DISPSHIFT       1

// cmd 0x08 : display enable
#define LCD_DISPCTL         0x08
#define LCD_DISPON          4
#define LCD_CURSON          2
#define LCD_CURSBLK         1

// cmd 0x10 : move cursor
#define LCD_MOVE            0x10
#define LCD_DISPSHFT        8
#define LCD_DISPDIR         4

// cmd 0x20 : set interface length
#define LCD_IFLEN           0x20
#define LCD_8BIT            16
#define LCD_2LINES          8
#define LCD_FNT5X10         4

// cmd 0x40 : set cgram address
#define LCD_CGADDR          0x40

// cmd 0x80 : set ddram address
#define LCD_DDADDR          0x80

#define lcd_wait() \
    delay_busy_us(96);delay_busy_us(96);delay_busy_us(96);delay_busy_us(96);delay_busy_us(96);delay_busy_us(96);delay_busy_us(96);delay_busy_us(96);delay_busy_us(48)

unsigned char smileyData0[] PROGMEM = {
    0x00, 0x00, 0x0A, 0x00, 0x00, 0x11, 0x0E, 0x00
}; // normal smiley
unsigned char smileyData1[] PROGMEM = {
    0x00, 0x00, 0x0A, 0x00, 0x00, 0x11, 0x1F, 0x00
}; // square smiley
unsigned char frownData[] PROGMEM = {
    0x00, 0x00, 0x0A, 0x00, 0x00, 0x0E, 0x11, 0x00
}; // frowny
unsigned char battData[] PROGMEM = {
    0x00, 0x0E, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x00
}; // battery low
unsigned char waitData[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00
}; // wait ...
unsigned char dlData[] PROGMEM = {
    0x00, 0x0E, 0x0E, 0x0E, 0x1F, 0x0E, 0x04, 0x00
}; // bootloader

uint8_t lcdPos = 0;
uint8_t lcdPosActual = 0;
uint8_t lcdClearFlag = 0;

char lcdContents[0x20];

void lcd_write(uint8_t is_data, uint8_t value) {
    // set data/ctrl
    LCD_RS(is_data);
    // write high nibble
    if (is_data) PORTD = (PORTD & 0x03) | (((value>>4)&0x0F)<<2) | _BV(LCD_PIN_E) | _BV(LCD_PIN_RS);
    else PORTD = (PORTD & 0x03) | (((value>>4)&0x0F)<<2) | _BV(LCD_PIN_E);
    //GPIO_WRITE_PORT((NIBBLE_HI(value)<<2) | _BV(LCD_PIN_E), LCD_PORT, 0x7C);
    lcd_wait();
    LCD_E(0);
    lcd_wait();
    // write low nibble
    if (is_data) PORTD = (PORTD & 0x03) | ((value&0x0F)<<2) | _BV(LCD_PIN_E) | _BV(LCD_PIN_RS);
    else PORTD = (PORTD & 0x03) | ((value&0x0F)<<2) | _BV(LCD_PIN_E);
    //GPIO_WRITE_PORT((NIBBLE_LO(value)<<2) | _BV(LCD_PIN_E), LCD_PORT, 0x7C);
    lcd_wait();
    LCD_E(0);
    lcd_wait();
}

void lcd_set_custom_char(uint8_t chnum,uint8_t *data) {
    uint8_t i;
    chnum <<= 3;
    for(i=0; i<8; i++) {
        lcd_write(LCD_CTRL, LCD_CGADDR|(chnum+i));
        lcd_write(LCD_DATA, pgm_read_byte(data+i));
    }
}

void lcd_init(void) {
    int i;
    delay_busy_ms(100);
    lcd_write(LCD_CTRL, 0x33); // should be? LCD_IFLEN|LCD_2LINES
    lcd_write(LCD_CTRL, 0x32); // should be? LCD_IFLEN|LCD_2LINES
    // no clue why the 0x33 and 0x32 should work
    //lcd_write(LCD_CTRL, LCD_IFLEN);
    //lcd_write(LCD_CTRL, LCD_IFLEN | LCD_2LINES);
    lcd_set_custom_char(1,(uint8_t*)smileyData0);
    lcd_set_custom_char(2,(uint8_t*)smileyData1);
    lcd_set_custom_char(3,(uint8_t*)frownData);
    lcd_set_custom_char(4,(uint8_t*)battData);
    lcd_set_custom_char(5,(uint8_t*)waitData);
    lcd_set_custom_char(6,(uint8_t*)dlData);
    lcd_write(LCD_CTRL, LCD_CLR); // 0x01
    lcd_write(LCD_CTRL, LCD_HOME); // 0x02
    for(i=0;i<0x20;i++)
        lcdContents[i] = ' ';
    lcd_write(LCD_CTRL, LCD_ENTRYMODE | LCD_CURSINC); // 0x06
    lcd_write(LCD_CTRL, LCD_DISPCTL | LCD_DISPON); // 0x0C
}

// apparently used only in bootloader, and there only for printable characters
void lcd_print(const char *string) {
    uint8_t i=0;
    while (*string) {
        if (*string=='\n') {
            lcd_write(LCD_CTRL, LCD_DDADDR);
            lcdPos=0;
        }
        else
            lcd_write(LCD_DATA, *string);
        if (i==15)
            lcd_write(LCD_CTRL, LCD_DDADDR|0x40);
        i++;
        lcdPos++;
        string++;
    }
}
