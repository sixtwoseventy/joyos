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
#import "packet.h"

@interface AppController : NSObject 
{
	int score;
	double timestamp;
    IBOutlet QCView* qcView;
    IBOutlet NSImageView* imageView;
	NSFileHandle* serialPort;
	packet_buffer position, lights;
}

void fill_goal(board_coord* pos, board_coord* last);
void generate_goal(board_coord* pos);

@end

#endif
