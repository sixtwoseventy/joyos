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

// Include headers from OS
#include <joyos.h>

#define SYNC_BYTE 0xE7

// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
	return 0;
}

// Entry point to contestant code.
// Create threads and return 0.
int
umain (void) {

	uint16_t index=0, syncCount=0;
	uint8_t sync = 0, payload_size=0;
	uint8_t cbuf;
	uint8_t pbuf[32];	

//	char msg[] = "6.270";
	
	// Loop forever
	while (1) {
		scanf("%c",&cbuf);
		
		if (cbuf==SYNC_BYTE) syncCount++;
		else syncCount = 0;

		if (syncCount >= 32){
			sync = 1;
			index = 0;
			payload_size = 0;
		}

		if (sync){
			if (index==0){
				if (cbuf != SYNC_BYTE){
					payload_size=cbuf;
					index++;
				}
			} else {
				pbuf[(index++)-1]=cbuf;

				if (index == (payload_size+1)){
					transmit_raw_packet(pbuf,payload_size);
//					transmit_raw_packet(msg,5);
//					transmit_position_packet(1,get_time()/1000.0,3.14159);
					index = 0;
//					printf("done\n");
				}
			}			

		}
	}

	// Will never return, but the compiler complains without a return
	// statement.
	return 0;
}

