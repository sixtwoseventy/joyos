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

// Boot failure message
char str_boot_fail[] PROGMEM  = "Init fail: %s\n\r";

// Boot text
char str_boot_message[] = "Happyboard v%X.%02X               \5";

// Boot progress messages
char str_boot_uart[] PROGMEM  = "UART0 opened at %d\n\r";
char str_boot_start[] PROGMEM = "Happyboard init started\n\r";
char str_boot_board[] PROGMEM = "Hardware version %X.%02X\n\r";
char str_boot_id[] PROGMEM    = "Board ID %04X\n\r";
char str_boot_conf[] PROGMEM  = "Board config OK\n\r";
char str_boot_batt[] PROGMEM  = "Battery OK: %dmV\n\r";
char str_boot_fpga[] PROGMEM  = "FPGA v%d.%d\n\r";

// Load board config.
// Return true if config CRC is valid
uint8_t
board_load_config (void) {
	uint8_t i;
	uint16_t crc=0;
	uint8_t *config_arr = (uint8_t*)&board_config;
	// read config from flash
	at45db_start_continuous_read(BOARD_CONFIG_ADDRESS);
	at45db_continuous_read_block(sizeof(board_config),(uint8_t*)(&board_config));
	at45db_end_continuous_read();
	// calculate CRC
	for (i=0;i<8;i++)
		crc = _crc_xmodem_update(crc,config_arr[i]);
	// check CRC
	return (crc==board_config.crc);
}

// Halt board on init failure and print message
void 
board_fail_P(PGM_P msg) {
	uart_printf_P(str_boot_fail,msg);
	// print message
	lcd_set_pos(16);
	lcd_printf(msg);
	lcd_set_pos(31);
	lcd_print_char('\3', NULL);
	// and stop
	while (1);
}

// Initialise board
void
board_init (void) {
	uint8_t fpgaOK, battOK, confOK;

	// Init GPIOs
	io_init();
	LED_PWR(1);
	// init uart
	uart_init(BAUD_RATE);
	stderr = &uartout;
	uart_printf_P(str_boot_uart,BAUD_RATE);
	uart_printf_P(str_boot_start);
	// other IO init
	digital_init();
	encoder_init();
	spi_init();
	motor_init();
	servo_init();
	lcd_init();
	stdout = &lcdout;
	adc_init();
	isr_init();

	// load config, or fail if invalid
	confOK = board_load_config();
	if (!confOK)
		board_fail_P(PSTR("Bad Config"));
	uart_printf_P(str_boot_conf);
	uart_printf_P(str_boot_board,
			board_config.version>>8,
			board_config.version&0xFF);
	uart_printf_P(str_boot_id, board_config.id);

	// print boot text to screen
	lcd_printf(str_boot_message, 
			board_config.version>>8, board_config.version&0xFF);

	// check battery, fail if <7.5V
	battOK = read_battery()>=7500;
	if (!battOK)
		board_fail_P(PSTR("Low battery"));
	uart_printf_P(str_boot_batt,read_battery());

	// initialise FPGA
	fpgaOK = fpga_init(FPGA_CONFIG_ADDRESS, board_config.fpga_len);
	if (!fpgaOK)
		board_fail_P(PSTR("FPGA failure"));
	uart_printf_P(str_boot_fpga,
			fpga_get_version_major(),
			fpga_get_version_minor());

	// all ok
	lcd_set_pos(31);
	lcd_print_char('\1', NULL);

	//delay_busy_ms(500);
	LED_COMM(0);
	// beep
	beep(450,2500);
}

