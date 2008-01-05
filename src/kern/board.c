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
#include <at45db011.h>
#include <util/crc16.h>

extern FILE lcdout, uartout;
#define BOOT_TEXT "Happyboard v"BOARD_VERSION_STRING"               \5"

uint8_t
board_load_config (void) {
	uint8_t i;
	uint16_t crc=0;
	uint8_t *config_arr = (uint8_t*)&board_config;
	at45db_start_continuous_read(BOARD_CONFIG_ADDRESS);
	at45db_continuous_read_block(sizeof(board_config),(uint8_t*)(&board_config));
	at45db_end_continuous_read();
	for (i=0;i<8;i++)
		crc = _crc_xmodem_update(crc,config_arr[i]);
	uart_printf("brd ver: 0x%04X\n\r",board_config.version);
	uart_printf("brd id: 0x%04X\n\r",board_config.id);
	uart_printf("fpga ver: 0x%04X\n\r",board_config.fpga_version);
	uart_printf("fpga len: 0x%04X\n\r",board_config.fpga_len);
	uart_printf("calc crc: 0x%04X\n\r",board_config.crc);
	uart_printf("crc: 0x%04X\n\r",crc);
	return (crc==board_config.crc);
}

void
board_init (void) {
	uint8_t fpgaOK, battOK, confOK;

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

	confOK = board_load_config();
	lcd_printf(BOOT_TEXT);
	fpgaOK = fpga_init(FPGA_CONFIG_ADDRESS, board_config.fpga_len);
	battOK = read_battery()>=7500;
	lcd_set_pos(16);
	if (!battOK)
		lcd_printf("Low battery    \3");
	if (!fpgaOK)
		lcd_printf("FPGA failure   \3");
	if ((!battOK) || (!fpgaOK)) {
		while (1);
	}
	lcd_set_pos(31);
	lcd_printf("\1");
	uart_printf("FPGA %d.%d\n\r",
			fpga_get_version_major(), 
			fpga_get_version_minor());

	delay_busy_ms(500);
	LED_COMM(0);
	// beep
	rf_init();
	beep(400,100);
}

