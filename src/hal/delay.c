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
#endif
#include "hal/delay.h"

// _delay_us maximum 96 us
// _delay_ms maximum 32 ms

#ifndef SIMULATE
#define _delay_loop_1_max_us 96
#define _delay_loop_1_max_ticks (uint8_t)(F_CPU*_delay_loop_1_max_us/3000000ul)
#define _delay_loop_2_max_ms 32
#define _delay_loop_2_max_ticks (uint16_t)(F_CPU*_delay_loop_2_max_ms/4000)
#endif

void delay_busy_us(uint32_t us) {

	#ifndef SIMULATE

    while(us > _delay_loop_1_max_us) {
        _delay_loop_1(_delay_loop_1_max_ticks);
        us -= _delay_loop_1_max_us;
    }
    _delay_loop_1((uint8_t)(F_CPU * us / 3000000ul));

	#else
	
	uint32_t wakeup_time = get_time_us() + us;
	while (get_time_us() < wakeup_time){
		yield();	
	}

	#endif

}

void delay_busy_ms(uint32_t ms) {

	#ifndef SIMULATE

    while(ms > _delay_loop_2_max_ms) {
        _delay_loop_2(_delay_loop_2_max_ticks);
        ms -= _delay_loop_2_max_ms;
    }
    _delay_loop_2((uint16_t)(F_CPU * ms / 4000));

	#else

	uint32_t wakeup_time = get_time() + ms;
	while (get_time() < wakeup_time){
		yield();	
	}
	
	#endif

}

