//
//  AppController.h
//  vision
//
//  Created by Peter Iannucci on 11/9/09.
//  Copyright __MyCompanyName__ 2009 . All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <Quartz/Quartz.h>
#import "packet.h"
#import "vision.h"

#define N_ROBOTS 4
#define N_MARGIN 2

@interface AppController : NSObject 
{
	int score;
	double timestamp;
    IBOutlet QCView* qcView;
    IBOutlet NSImageView* imageView;
    IBOutlet NSTextField* teamA, *teamB;
	NSFileHandle* serialPort;
	packet_buffer position, lights, start, stop;
	
	// variables related to lighting experiments
	int currentRobot;
	NSTimeInterval lastTime;
	sighting oldrobot[N_ROBOTS];
	volatile bool killTickThread;
	volatile NSThread *tickThread;
}

- (IBAction)reset:(id)sender;
void fill_goal(board_coord* pos, board_coord* last);
void generate_goal(board_coord* pos);

@end