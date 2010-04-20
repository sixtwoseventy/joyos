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

#ifndef SIMLUATE

#include <kern/global.h>
#include <kern/thread.h>
#include <mcp3008.h>

#else

#include <joyos.h>

#endif

uint16_t analog_read(uint8_t port) {
	
	#ifndef SIMULATE

    uint16_t v;
    if (port>=8 && port<16) {
        // keep trying until sample is successful...
        while (mcp3008_get_sample(MCP3008_ADC1, MCP3008_CH0+(15-port), &v)
                != MCP3008_SUCCESS) {
            // XXX: for now notify user of fail
            panic("mcp3008 fail");

            // yield and try again
            yield();
        }
    }

    else if (port>=16 && port<24) {
        while (mcp3008_get_sample(MCP3008_ADC2, MCP3008_CH0+(23-port), &v)
                != MCP3008_SUCCESS) {
            // XXX: for now notify user of fail
            panic("mcp3008 fail");

            // yield and try again
            yield();
        }
    }

    else {
        panic("analog_read");
    }

    return v;

	#else

	return 0;

	#endif

}

