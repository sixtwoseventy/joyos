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

#include "config.h"
#include "spi.h"
#include "mcp3008.h"
#include <global.h>

#define FLAGS	(SPI_FLAG_MSB_FIRST | SPI_FLAG_LOW_IDLE | \
		 SPI_FLAG_SAMP_LEADING | SPI_FLAG_DIV_CLK_16)

int8_t mcp3008_get_sample (mcp3008_input config, uint16_t * sample)
{
	uint8_t cmd[3];

	if (spi_acquire() == SPI_IN_USE)
		return MCP3008_SPI_BUSY;

	cmd[0] = 0x40 | (config << 2);

	spi_set_master (FLAGS);

	SPI_MOTOR_SS(0);
	spi_transfer_sync (cmd, 3);
	SPI_MOTOR_SS(1);

	*sample = ((uint16_t)cmd[1] << 8) | cmd[2];
	*sample >>= 3;

	spi_release();

	return MCP3008_SUCCESS;
}


static uint8_t cmdbuf[3];
static mcp3008_cb sample_cb;


static void
sample_done (uint8_t * buf, uint8_t len, void * user)
{
	uint16_t val;
	mcp3008_cb cb = sample_cb;
	
	val = ((uint16_t)buf[1] << 8) | buf[2];
	cb (val >> 3, user);
}

void mcp3008_sample_async (mcp3008_input config, mcp3008_cb cb,
		void * user)
{
	panic ("mcp3008_sample_async");
	spi_set_master (FLAGS);

	sample_cb = cb;
	cmdbuf[0] = 0x40 | (config << 2);

	spi_transfer_async (cmdbuf, 3, sample_done, user);
}
