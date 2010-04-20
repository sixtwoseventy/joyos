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

#include "config.h"
#include "hal/io.h"
#include "hal/spi.h"
#include "hal/delay.h"
#include "at45db011.h"

#define STAT_READY              7
#define STAT_COMP               6

#define CMD_BLOCK_ERASE         0x50
#define CMD_PAGE_TO_BUF_TXF     0x53
#define CMD_PAGE_REWRITE        0x58
#define CMD_PAGE_TO_BUF_CMP     0x60
#define CMD_PAGE_ERASE          0x81
#define CMD_PROGRAM_VIA_BUF     0x82
#define CMD_BUF_PROGRAM_ERASE   0x83
#define CMD_BUF_WRITE           0x84
#define CMD_BUF_PROGRAM_NOERASE 0x88
#define CMD_PAGE_READ           0xD2
#define CMD_BUFFER_READ         0xD4
#define CMD_STATUS_REG          0xD7
#define CMD_CONTINUOUS_READ     0xE8

#define PAGE_EXTRA_BYTES        8
#define PAGE_OFFSET_MASK        0x000ff
#define PAGE_ADDR_SHIFT         8

uint8_t read_offset;

static void wait_for_ready () {
    uint8_t byte;

    byte = CMD_STATUS_REG;

    SPI_FLASH_SS (0);
    spi_transfer_sync (&byte, 1);
    do {
        spi_transfer_sync (&byte, 1);
    } while (!(byte & _BV(STAT_READY)));
    SPI_FLASH_SS (1);
    delay_busy_us(1);
}

int8_t at45db_fill_buffer_from_flash(uint32_t addr) {
    uint8_t cmd[4];

    if (spi_acquire() == SPI_IN_USE)
        return AT45DB_SPI_BUSY;

    spi_set_master (SPI_CLK_DIV_2, SPI_FLAGS_DEFAULT);

    cmd[0] = CMD_PAGE_TO_BUF_TXF;
    cmd[1] = (addr >> PAGE_ADDR_SHIFT) >> 7;
    cmd[2] = (addr >> PAGE_ADDR_SHIFT) << 1;
    cmd[3] = 0;

    wait_for_ready ();

    SPI_FLASH_SS (0);
    spi_transfer_sync (cmd, 4);
    SPI_FLASH_SS (1);
    delay_busy_us (1);

    spi_release ();

    return AT45DB_READY;
}

int8_t at45db_start_continuous_read (uint32_t addr) {
    uint8_t cmd[4];

    if (spi_acquire() == SPI_IN_USE)
        return AT45DB_SPI_BUSY;

    spi_set_master (SPI_CLK_DIV_2, SPI_FLAGS_DEFAULT);

    cmd[0] = CMD_CONTINUOUS_READ;
    cmd[1] = (addr >> PAGE_ADDR_SHIFT) >> 7;
    cmd[2] = (addr >> PAGE_ADDR_SHIFT) << 1;
    cmd[3] = read_offset = addr & PAGE_OFFSET_MASK;

    wait_for_ready ();

    SPI_FLASH_SS (0);
    /* transmit the 4 command bytes */
    spi_transfer_sync (cmd, 4);

    /* transmit 4 don't care bytes */
    spi_transfer_sync (cmd, 4);

    return AT45DB_READY;
}

uint8_t at45db_get_next_byte () {
    uint8_t byte;
    spi_transfer_sync (&byte, 1);

    /* skip the extra 8 bytes at the end of each page */
    if (read_offset == AT45DB_PAGE_SIZE - 1) {
        uint8_t scratch, i;
        for (i = 0; i < PAGE_EXTRA_BYTES; i++)
            spi_transfer_sync (&scratch, 1);
    }
    read_offset++;

    return byte;
}

uint8_t at45db_continuous_read_block(uint16_t len, uint8_t* data) {
    uint16_t i;
    for (i=0;i<len;i++)
        data[i] = at45db_get_next_byte();

    return AT45DB_READY;
}

void at45db_end_continuous_read () {
    SPI_FLASH_SS (1);
    delay_busy_us (1);
    spi_release ();
}

int8_t at45db_get_status (uint8_t * status) {
    uint8_t cmd[2];

    cmd[0] = CMD_STATUS_REG;
    cmd[1] = 0;

    if (spi_acquire() == SPI_IN_USE)
        return AT45DB_SPI_BUSY;

    spi_set_master (SPI_CLK_DIV_2, SPI_FLAGS_DEFAULT);

    SPI_FLASH_SS (0);
    spi_transfer_sync (cmd, 2);
    SPI_FLASH_SS (1);
    delay_busy_us (1);

    spi_release ();

    *status = cmd[1];

    return AT45DB_READY;
}

int8_t at45db_store_buffer (uint32_t addr) {
    uint8_t cmd[4];

    if (spi_acquire() == SPI_IN_USE)
        return AT45DB_SPI_BUSY;

    spi_set_master (SPI_CLK_DIV_2, SPI_FLAGS_DEFAULT);

    cmd[0] = CMD_BUF_PROGRAM_ERASE;
    cmd[1] = (addr >> PAGE_ADDR_SHIFT) >> 7;
    cmd[2] = (addr >> PAGE_ADDR_SHIFT) << 1;
    cmd[3] = 0;

    wait_for_ready ();

    /* transmit the 4 command bytes */
    SPI_FLASH_SS (0);
    spi_transfer_sync (cmd, 4);
    SPI_FLASH_SS (1);
    delay_busy_us (1);

    spi_release ();

    return AT45DB_READY;
}

int16_t at45db_fill_buffer (uint32_t addr, uint8_t * data, uint16_t len) {
    uint8_t cmd[4];
    uint16_t i;
    uint8_t offset;

    if (spi_acquire() == SPI_IN_USE)
        return AT45DB_SPI_BUSY;

    spi_set_master (SPI_CLK_DIV_2, SPI_FLAGS_DEFAULT);

    cmd[0] = CMD_BUF_WRITE;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = offset = addr & PAGE_OFFSET_MASK;

    SPI_FLASH_SS (0);
    /* transmit the 4 command bytes */
    spi_transfer_sync (cmd, 4);

    if ((uint16_t)offset + len > AT45DB_PAGE_SIZE)
        len = AT45DB_PAGE_SIZE - offset;

    for (i = 0; i < len; i++) {
        cmd[0] = data[i];
        spi_transfer_sync (cmd, 1);
    }
    SPI_FLASH_SS (1);
    delay_busy_us (1);

    spi_release ();

    return len;
}

#endif

