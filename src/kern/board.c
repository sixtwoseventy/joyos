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

#include <config.h>
#include <board.h>
#include <hal/io.h>
#include <hal/spi.h>
#include <hal/adc.h>
#include <hal/delay.h>
#include <kern/global.h>
#include <kern/lock.h>
#include <kern/isr.h>

extern FILE lcdout, uartout;
#define BOOT_TEXT "Happyboard v"BOARD_VERSION_STRING"               \5"

void
board_init (void) {
	io_init();
	LED_PWR(1);
	digital_init();
	encoder_init();
	spi_init();
	motor_init();
	servo_init();
	lcd_init();
	adc_init();
	uart_init(BAUD_RATE);
	uart_print(BOOT_TEXT);
	uart_print("\n\r");
	isr_init();
	// init stdout/stderr
	stdout = &lcdout;
	stderr = &uartout;
	
	lcd_printf(BOOT_TEXT);
	lcd_set_pos(31);
	// init fpga
	uint8_t fpgaOK = fpga_init();
	uint8_t battOK = read_battery()>=7500;
	// smiley / frowny depending on fpga state
	lcd_printf(fpgaOK ? "\1" : "\3");
	uart_printf("FPGA %d %d.%d\n\r",
			fpgaOK,
			fpga_get_version_major(), 
			fpga_get_version_minor());
	lcd_set_pos(16);
	if (!battOK)
		lcd_printf("Low battery    \3");
	if (!fpgaOK)
		lcd_printf("FPGA failure   \3");
	if ((!battOK) || (!fpgaOK)) {
		while (1);
	}

	delay_busy_ms(500);
	LED_COMM(0);
	// beep
	rf_init();
	beep(400,100);
}

