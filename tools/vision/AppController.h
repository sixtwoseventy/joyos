//
//  AppController.h
//  vision
//
//  Created by Peter Iannucci on 11/9/09.
//  Copyright __MyCompanyName__ 2009 . All rights reserved.
//

#pragma once

#ifndef _APP_CONTROLLER_H
#define _APP_CONTROLLER_H

#import <Cocoa/Cocoa.h>

#import <Quartz/Quartz.h>
#define PAYLOAD_SIZE 30

typedef struct {
	uint8_t id;
	signed x : 12;
	signed y : 12;
	signed theta : 12;
	unsigned confidence : 12;
} __attribute__ ((aligned (1))) __attribute__ ((packed)) board_coord;

typedef struct{
	uint8_t type;
	uint8_t address;
	uint8_t payload[30];
} packet;

typedef struct {
	uint8_t type;
	uint8_t address;
	union {
		uint8_t array[PAYLOAD_SIZE];
		board_coord coords[4];
	} payload;
} __attribute__ ((packed)) packet_buffer;

extern packet_buffer tx, rx;

@interface AppController : NSObject 
{
	int score;
	double timestamp;
    IBOutlet QCView* qcView;
    IBOutlet NSImageView* imageView;
	NSFileHandle* serialPort;
}

void fill_goal(board_coord* pos, board_coord* last);
void generate_goal(board_coord* pos);

@end

#endif
