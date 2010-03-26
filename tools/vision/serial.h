/*
 *  serial.h
 *  vision
 *
 *  Created by 6. 270 on 1/17/10.
 *  Copyright 2010 Massachusetts Institute of Technology. All rights reserved.
 *
 */


NSFileHandle *open_serial(NSString *path);
void sync_serial(NSFileHandle *fh);
void send_packet(NSFileHandle *fh, void *packet, uint8_t length);
