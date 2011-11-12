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
#include <config.h>
#endif
#include <board.h>
#ifndef SIMULATE
#include <hal/io.h>
#include <hal/spi.h>
#include <hal/adc.h>
#endif
#include <hal/delay.h>
#include <kern/global.h>
#include <kern/lock.h>
#ifndef SIMULATE
#include <kern/isr.h>
#include <kern/memlayout.h>
#endif
#include <kern/thread.h>
#ifndef SIMULATE
#include <at45db011.h>
#include <util/crc16.h>
#else
#include <stdio.h>
#endif

#ifndef SIMULATE

extern FILE lcdout, uartio;

#endif

// Boot failure message
#define str_boot_fail "Init fail: %S\n"

// Boot text
#define str_boot_message "Happyboard v%X.%02X\n"

// Boot progress messages
#define str_boot_uart "UART0 opened at %d\n"
#define str_boot_start "Happyboard init started\n"
#define str_boot_board "Hardware version %X.%02X\n"
#define str_boot_id "Board ID %04X\n"
#define str_boot_conf "Board config OK\n"
#define str_boot_batt "Motor Battery: %dmV\n"
#define str_boot_fpga "FPGA v%d.%d\n"

// Load board config.
// Return true if config CRC is valid
uint8_t board_load_config (void) {

	#ifndef SIMULATE

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

	#else

	printf("Skipping board config check...\n");
	return 1;

	#endif

}

#ifndef SIMULATE
#define board_fail(msg) board_fail_P(PSTR(msg))
// Halt board on init failure and print message
void board_fail_P(PGM_P msg) {
    printf(str_boot_fail, msg);
    // print message
#ifdef LCD_DEBUG
    lcd_set_pos(16);
    lcd_printf(msg);
    lcd_set_pos(31);
    lcd_print_char('\3', NULL);
#endif
    // and stop
    while (1) {
        for (int i = 0; i < 2; i++) {
            LED_COMM(1);
            pause(100);
            LED_COMM(0);
            pause(150);
        }
        pause(800);
    }
}
#else
#define board_fail(msg) board_fail_P(msg)
// Halt board on init failure and print message
void board_fail_P(char* msg) {
	// print message
    printf(str_boot_fail, msg);
    // and stop
    while (1);
}
#endif

#ifndef SIMULATE

#define EXTERNAL_RAM

void memory_init(void) {
#ifndef EXTERNAL_RAM
    __malloc_heap_end = (void*)STACKTOP(MAX_THREADS);
#else
    __malloc_heap_start = (void*)0x8000;
    __malloc_heap_end = (void*)0xFFFF;
#endif
    printf ("__malloc_heap_start = %p\n", __malloc_heap_start);
    printf ("__malloc_heap_end = %p\n", __malloc_heap_end);

    uint16_t heap_size = (__malloc_heap_end - __malloc_heap_start);
    printf("heap size: %u bytes\n", heap_size);

    if (__malloc_heap_start >= __malloc_heap_end)
        panic("memory full");
}

#endif

// Initialise board
void board_init (void) {

	#ifndef SIMULATE
    io_init(); // Init GPIOs
    uart_init(BAUD_RATE);
    stderr = &uartio;
    printf(str_boot_uart,BAUD_RATE);
    printf(str_boot_start);
	#else
	printf("Skipping UART initialization...\n");
	#endif
	#ifndef SIMULATE
    digital_init();
	#endif
    encoder_init();
	#ifndef SIMULATE
    spi_init();
    motor_init();
    servo_init();
#ifdef LCD_DEBUG
    lcd_init(); //consider wrapping this in an #ifdef LCD_DEBUG tag?
    stdout = &lcdout;
#else
    stdout = &uartio;
    stdin = &uartio;
#endif
    adc_init();
    isr_init();
    memory_init();
	#endif

    // load config, or fail if invalid
    if (!board_load_config())
        board_fail("Bad Config");
    printf(str_boot_conf);
    printf(str_boot_board,
            board_config.version>>8,
            board_config.version&0xFF);
    printf(str_boot_id, board_config.id);

    // print boot text to screen
    printf(str_boot_message, board_config.version>>8, board_config.version&0xFF);

    // check battery, fail if <7.5V
    printf(str_boot_batt,read_battery());
#ifdef CHECK_BATTERY
    if (!(read_battery()>=7200)) {
        // NOTE: in the current 2-battery version of the HappyBoard, the 
        // battery voltage is the motor battery (P+).  Holding GO overrides
        // the check so you can run the HappyBoard without a motor battery.
        if (go_press())
            printf("WARNING: LOW BATTERY\n");
        else 
            board_fail("Low battery");
    } else {
        printf("Battery OK\n");
    }
#endif

	#ifndef SIMULATE
    // initialise FPGA
    if (!fpga_init(FPGA_CONFIG_ADDRESS, board_config.fpga_len))
        board_fail("FPGA failure");
    printf(str_boot_fpga, fpga_get_version_major(), fpga_get_version_minor());
	#else
	printf("Skipping FPGA initialization...\n");
	#endif

    // all ok
#ifndef SIMULATE
#ifdef LCD_DEBUG
    lcd_set_pos(31);
    lcd_print_char('\1', NULL);
#else
	printf("Board init complete.\n");
#endif
#else
    printf("Board init complete.\n");
#endif

#ifndef SIMULATE
    LED_COMM(0);
#endif

}
