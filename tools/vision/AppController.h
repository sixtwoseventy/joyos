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
#define N_MARGIN 4

@interface AppController : NSObject 
{
	int score;
	double timestamp;
    IBOutlet QCView* qcView;
    IBOutlet NSTextField* teamA, *teamB, *clock;
	IBOutlet NSButton *checkBox;
	NSFileHandle* serialPort;
	volatile packet_buffer position, lights, start, stop;
	
	// variables related to lighting experiments
	int currentRobot;	
	NSTimeInterval lastTime;
	NSTimeInterval startTime, endTime;
	sighting oldrobot[N_ROBOTS];
	volatile bool killTickThread;
	volatile NSThread *tickThread;
	NSSpeechSynthesizer *recog;
	NSLock *tickLock;
	int transmittedStartPacket, transmittedStopPacket;
}

- (IBAction)reset:(id)sender;
- (IBAction)extendCalibrationPeriod:(id)sender;
void fill_goal(board_coord* pos, board_coord* last);
void generate_goal(board_coord* pos);

@end