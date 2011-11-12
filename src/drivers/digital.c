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
#include <fpga.h>
#include <analog.h>
#include <kern/lock.h>
#include <kern/global.h>

#else

#include <joyos.h>

#endif

#ifndef SIMULATE

struct lock digital_lock;

void digital_init () {
    init_lock(&digital_lock, "digital lock");
}

#endif

// Pin modes of digital IO: 1 = output
uint8_t output_pinmode = 0;

uint8_t digital_read_8() {

	#ifndef SIMULATE

    acquire(&digital_lock);
    // read from the FPGA all 8 digital ports
    uint8_t result = ~(fpga_read_byte(FPGA_DIGITAL_BASE));
    release(&digital_lock);
    return result;

	#else

	return 0;

	#endif

}

// must be holding digital_lock
static inline void _set_digital_pinmode(uint8_t port, uint8_t output) {
    if (port > 7) {
        panic("_set_diital_pinmode");
    }

    if (output) {
        output_pinmode |= (1 << port);
    } else {
        output_pinmode &= ~(1 << port);
    }

    fpga_write_byte(FPGA_DIGITAL_PINMODE, output_pinmode);
}

uint8_t digital_read(uint8_t port) {

	#ifndef SIMULATE

    uint8_t result = 0;
    if (port<8) {
        acquire(&digital_lock);
        _set_digital_pinmode(port, 0);

        // grab a byte from the FPGA and mask the port's bit...
        result = ((~(fpga_read_byte(FPGA_DIGITAL_BASE)))>>port)&1;
        release(&digital_lock);
    } else if (port<24) {
        // is analog value of `port' greater than half the max+1 ?
        result = analog_read(port)>((ANALOG_MAX+1)>>1);
    } else {
        panic("digital_read");
    }

    return result;

	#else

	return 0;

	#endif

}

void digital_write_pwm(uint8_t port, uint8_t val) {
    #ifndef SIMULATE
    
    if (port>7) {
        panic("digital_write");
    }

    acquire(&digital_lock);

    // write the new value first, then set the pin as an output
    fpga_write_byte(FPGA_DIGITAL_PWM_BASE + port*FPGA_DIGITAL_SIZE, val);
    _set_digital_pinmode(port, 1);
    
    release(&digital_lock);
    #endif
}

void digital_write(uint8_t port, uint8_t on) {
    digital_write_pwm(port, on ? 255 : 0);
}

