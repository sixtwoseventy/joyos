/*
 *  serial.c
 *  vision
 *
 *  Created by 6. 270 on 1/17/10.
 *  Copyright 2010 Massachusetts Institute of Technology. All rights reserved.
 *
 */

#import "serial.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

NSFileHandle *open_serial(NSString *path) {
	struct termios options;
	
	int fildes = open([path cStringUsingEncoding:NSASCIIStringEncoding], O_RDWR | O_NONBLOCK);
	memset(&options,0,sizeof(struct termios));
	cfmakeraw(&options);
	cfsetspeed(&options, 19200);
	options.c_cflag = CREAD | CLOCAL;
	options.c_cflag |= CS8;
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 10;
	ioctl(fildes, TIOCSETA, &options);
	NSFileHandle *fh = [[NSFileHandle alloc] initWithFileDescriptor:fildes closeOnDealloc:YES];
	
	[fh readInBackgroundAndNotify];
	return fh;
}

void sync_serial(NSFileHandle *fh) {
	const int length=32;
	const int sync_byte=0;
	uint8_t sync[length];
	for (int i=0; i<length; i++)
		sync[i] = sync_byte;
	[fh writeData:[NSData dataWithBytesNoCopy:sync length:length freeWhenDone:NO]];
}

void send_packet(NSFileHandle *fh, void *packet, uint8_t length) {
	@synchronized(fh) {
		[fh writeData:[NSData dataWithBytesNoCopy:&length length:1 freeWhenDone:NO]];
		[fh writeData:[NSData dataWithBytesNoCopy:packet length:length freeWhenDone:NO]];
		usleep(20000);
	}
}
