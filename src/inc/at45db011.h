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

#ifndef _AT45DB011_H_
#define _AT45DB011_H_

#define AT45DB_PAGE_SIZE    256

#define AT45DB_READY        0
#define AT45DB_SPI_BUSY     -1

int8_t at45db_start_continuous_read (uint32_t addr);
uint8_t at45db_continuous_read_block(uint16_t len, uint8_t* data);
void at45db_end_continuous_read();
uint8_t at45db_get_next_byte();
int8_t at45db_get_status(uint8_t * status);
int8_t at45db_store_buffer(uint32_t addr);
int16_t at45db_fill_buffer(uint32_t addr, uint8_t * data, uint16_t len);
int8_t at45db_fill_buffer_from_flash(uint32_t addr);

#endif

#endif
