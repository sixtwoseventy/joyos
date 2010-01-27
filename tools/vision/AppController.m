//
//  AppController.m
//  vision
//
//  Created by Peter Iannucci on 11/9/09.
//  Copyright __MyCompanyName__ 2009 . All rights reserved.
//

#import "AppController.h"
#import "math.h"
#import "serial.h"
#import "vision.h"

@implementation AppController

- (void) gotDataNotification:(NSNotification *)notification {
	fflush(stdout);
	NSData *data = [[notification userInfo] objectForKey:NSFileHandleNotificationDataItem];
	[(NSFileHandle*)[NSFileHandle fileHandleWithStandardOutput] writeData:data];
	[serialPort readInBackgroundAndNotify];
}

- (void) awakeFromNib
{
	if(![qcView loadCompositionFromFile:[[NSBundle mainBundle] pathForResource:@"Vision" ofType:@"qtz"]]) {
		NSLog(@"Could not load composition");
	}
	
	serialPort = open_serial(@"/dev/tty.usbserial-A800cBag");
	killTickThread = NO;
	tickThread = nil;
	[self reset:nil];
	[self performSelectorInBackground:@selector(tickThread:) withObject:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gotDataNotification:) name:NSFileHandleReadCompletionNotification object:nil];

	NSMutableArray *cmds = [NSMutableArray arrayWithObjects:@"Reset board", @"Team A", @"Team B", nil];
	for (int i=0; i<32; i++)
		[cmds addObject:[NSString stringWithFormat:@"Number %d", i]];
	recog = [[NSSpeechRecognizer alloc] init];
	[recog setCommands:cmds];
	[recog setDelegate:self];
	[recog startListening];
}

- (void)speechRecognizer:(NSSpeechRecognizer *)sender didRecognizeCommand:(NSString *)aCmd {
	NSLog(@"%@", aCmd);
	static int teamAB = 0;
	if ([aCmd isEqualToString:@"Team A"])
		teamAB = 1;
	else if ([aCmd isEqualToString:@"Team B"])
		teamAB = 2;
	else if (![aCmd isEqualToString:@"Reset board"] && teamAB != 0) {
		int i;
		sscanf([aCmd cStringUsingEncoding:NSASCIIStringEncoding], "Number %d", &i);
		[teamAB==1 ? teamA : teamB setIntValue: i];
	} else {
		[self reset:nil];
	}

}

- (IBAction)reset:(id)sender {
	if (tickThread) {
		killTickThread = YES;
		usleep(100000);
	}
	
	sync_serial(serialPort);
	
	position.type = POSITION;
	position.address = 0xFF;

	lights.type = LIGHT;
	lights.address = 0xFF;
	
	for (int i=0; i<4; i++)
		lights.payload.lights[i].value = 0;
	
	lights.payload.lights[0].id = position.payload.coords[0].id = [teamA integerValue];
	lights.payload.lights[1].id = position.payload.coords[1].id = [teamB integerValue];
	lights.payload.lights[2].id = position.payload.coords[2].id = 128;
	lights.payload.lights[3].id = position.payload.coords[3].id = 129;
	
	score = 0;
	timestamp = [[NSDate date] timeIntervalSince1970];
	
	for (int i=0; i<N_ROBOTS; i++) {
		oldrobot[i].x = oldrobot[i].y = oldrobot[i].theta = 0.f;
		oldrobot[i].id = position.payload.coords[i].id;
	}
	
	currentRobot = 0;
	lights.payload.lights[currentRobot].value = 150;
	send_packet(serialPort,&lights,sizeof(packet_buffer));
	
	// start robots
	start.type = START;
	for (int i=0; i<N_ROBOTS; i++){
		start.payload.array[i] = position.payload.coords[i].id;
	}
	
	send_packet(serialPort,&start,sizeof(packet_buffer));
	killTickThread = NO;
	[self performSelectorInBackground:@selector(tickThread:) withObject:nil];
}

- (void) dealloc {
	[serialPort release];
	[super dealloc];
}

int16_t rand_coord() {
	int16_t coord;
	
	do{
		coord = (random()-RAND_MAX/2) / (RAND_MAX / (1<<12));
	} while((coord < -(1<<11)+512) || (coord > (1<<11)-512));
	
	return coord;
}

void generate_goal(board_coord* pos) {
	pos->x = rand_coord();
	pos->y = rand_coord() * 3/4;
}

void fill_goal(board_coord* pos, board_coord* last) {
	do {
		generate_goal(pos);
	} while (((long)(pos->x-last->x))*((long)(pos->x-last->x)) +
			 ((long)(pos->y-last->y))*((long)(pos->y-last->y)) < (1L << 22));
}

BOOL close_to(board_coord* pos1, board_coord* pos2) {
	return ((long)(pos1->x-pos2->x))*((long)(pos1->x-pos2->x)) +
		   ((long)(pos1->y-pos2->y))*((long)(pos1->y-pos2->y)) < (1L << 16); // 6 inches
}

float angleDiff(float a, float b) {
	float x = cos(a) - cos(b);
	float y = sin(a) - sin(b);
	return x*x+y*y;
}

- (void)tickThread:(id)arg {
	tickThread = [NSThread currentThread];
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(tick:) userInfo:nil repeats:YES];
	
    while (!killTickThread) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.01]];
    }
	
    [pool release];
}

- (void)snapTo:(NSValue *)pointer {
	*((NSBitmapImageRep **)[pointer pointerValue]) = [[qcView valueForOutputKey:@"Blurred" ofType:@"NSBitmapImageRep"] copy];
	// createSnapshotImageOfType:@"NSBitmapImageRep"];
}

void visualize(NSBitmapImageRep *bitmap, unsigned char *data,
			   int bytesPerRow, int bytesPerPixel,
			   int width, int height, NSImageView *imageView) {
	NSImage *image = [[NSImage alloc] init];
	
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++) {
			if (!data[x*bytesPerPixel + y*bytesPerRow+0]) {
				data[x*bytesPerPixel + y*bytesPerRow+1] = 0;
				data[x*bytesPerPixel + y*bytesPerRow+2] = 0;
				data[x*bytesPerPixel + y*bytesPerRow+3] = 0;
			} else {
				data[x*bytesPerPixel + y*bytesPerRow+1] = 255;
				data[x*bytesPerPixel + y*bytesPerRow+2] = 255;
				data[x*bytesPerPixel + y*bytesPerRow+3] = 255;
			}
			
			data[x*bytesPerPixel + y*bytesPerRow+0] = 255;
		}
	} 
	
	[image addRepresentation:bitmap];
	
	[imageView setImage: image];
	[image release];
	[bitmap release];
}

void reorder(sighting robot[], sighting oldrobot[], float diagLengthSquared) {
	// make an effort to figure out which is which
	// assignments is a map of new robots -> old robots
	int assignments[N_ROBOTS+N_MARGIN];
	float distance[N_ROBOTS+N_MARGIN][N_ROBOTS];
	for (int i=0; i<N_ROBOTS+N_MARGIN; i++) {
		for (int j=0; j<N_ROBOTS; j++) {
			float dx = robot[i].x - oldrobot[j].x;
			float dy = robot[i].y - oldrobot[j].y;
			float dtheta_squared = angleDiff(robot[i].theta, oldrobot[j].theta);
			if (isnan(dx) || isnan(dy))
				dx = dy = sqrt(diagLengthSquared);
			if (isnan(dtheta_squared))
				dtheta_squared = diagLengthSquared;
			
			float oldUnConfSquared = oldrobot[i].regionMax < ROBOT_THRESH ? diagLengthSquared : 0;
			float newUnConfSquared = robot[i].regionMax < ROBOT_THRESH ? diagLengthSquared : 0;
			
			distance[i][j] = dx*dx+dy*dy+dtheta_squared*R*R/4.f+oldUnConfSquared+newUnConfSquared;
			assert(!isnan(distance[i][j]));
		}
		assignments[i] = -1;
	}
	for (int k=0; k<N_ROBOTS; k++) {
		float minDistance = INFINITY;
		int imin, jmin;
		for (int i=0; i<N_ROBOTS+N_MARGIN; i++) {
			for (int j=0; j<N_ROBOTS; j++) {
				if (distance[i][j] < minDistance) {
					imin = i;
					jmin = j;
					minDistance = distance[i][j];
				}
			}
		}
		assignments[imin] = jmin;
		for (int i=0; i<N_ROBOTS+N_MARGIN; i++)
			distance[i][jmin] = INFINITY;
		for (int j=0; j<N_ROBOTS; j++)
			distance[imin][j] = INFINITY;
	}
	// reassign points according to assignments
	for (int i=0; i<N_ROBOTS; i++) {
		int j;
		for (j=i; j<N_ROBOTS+N_MARGIN; j++) {
			if (assignments[j] == i)
				break;
		}
		if (j == N_ROBOTS+N_MARGIN)
			printf("This should never print.  No, really.\n"); // how did this happen?
		if (i != j) {
			SWAP(robot[i], robot[j]);
			SWAP(assignments[i], assignments[j]);
		}
	}
	for (int i=0; i<N_ROBOTS; i++)
		robot[i].id = oldrobot[i].id;	
}

void experiment(sighting robot[], int *currentRobot, NSTimeInterval *lastTime, packet_buffer *lights, NSFileHandle *serialPort) {
	// experiment consists of commanding one robot to turn its light on.
	// then we test which 't' got the brightest and swap the robot->t 
	// assignment as necessary. evaluate the results of each experiment
	// after each second, then increment currentRobot.
	
	NSTimeInterval now = [[NSDate date] timeIntervalSinceReferenceDate];
	lights->payload.lights[*currentRobot].value = (now-*lastTime) < 0.1 ? 0 : 180;
	if ((now-*lastTime) > .8){
		// evaluate results of the experiment
		int litRobot=0, litRobots=0;
		for (int i=0; i<N_ROBOTS; i++) {
			if (robot[i].light) {
				litRobot = i;
				litRobots++;
			}
		}
		if (litRobots == 1) {
			// the id of robot[litRobot] should be lights->payload.lights[currentRobot].id
			// and litRobot should be currentRobot
			if (robot[litRobot].id != lights->payload.lights[*currentRobot].id) {
				printf("Found robot id %d while looking for robot id %d -- swapping\n", robot[litRobot].id, lights->payload.lights[*currentRobot].id);
				for (int i=0; i<N_ROBOTS; i++) {
					if (robot[i].id == lights->payload.lights[*currentRobot].id) {
						SWAP(robot[i].id, robot[litRobot].id);
						break;
					}
				}
			}
			if (litRobot != *currentRobot) {
				printf("Found robot index %d while looking for robot index %d -- swapping\n", litRobot, *currentRobot);
				SWAP(robot[litRobot], robot[*currentRobot]);
			}
		} else{
			printf("reject experiment\n");
		}
		
		// start the next experiment
		lights->payload.lights[*currentRobot].value = 0;
		*currentRobot = (++*currentRobot) % N_ROBOTS;
		
		*lastTime = now;
	}
	send_packet(serialPort,lights,sizeof(packet_buffer));
}

void report(sighting robot[], packet_buffer *position, NSFileHandle *serialPort) {
	for (int i=0; i<N_ROBOTS; i++){
		position->payload.coords[i].x = (int16_t)((robot[i].x-320)*(1<<12)/640.);
		position->payload.coords[i].y = -(int16_t)((robot[i].y-240)*(1<<12)/640.);
		position->payload.coords[i].theta = -(int)(robot[i].theta*(1<<12)/(M_PI*2));
		position->payload.coords[i].confidence = robot[i].regionMax;
		position->payload.coords[i].id = robot[i].id;
	}
	
	send_packet(serialPort,position,sizeof(packet_buffer));
}

- (void)tick:(id)arg {	
	NSBitmapImageRep *bitmap = 0;
	[self performSelectorOnMainThread:@selector(snapTo:) withObject:[NSValue valueWithPointer:&bitmap] waitUntilDone:YES];
    NSSize imgSize = [bitmap size];
	
	unsigned char *data = [bitmap bitmapData];
	int bytesPerPixel = [bitmap bitsPerPixel] >> 3;
	int bytesPerRow = [bitmap bytesPerRow];

	float diagLengthSquared = ((float)imgSize.width)*((float)imgSize.width)+((float)imgSize.height)*((float)imgSize.height);
	
	sighting robot[N_ROBOTS+N_MARGIN];
	
	for (int i=0; i<N_ROBOTS+N_MARGIN; i++)
		findRobot(data, bytesPerRow, bytesPerPixel, imgSize.width, imgSize.height, &robot[i]);

	reorder(robot, oldrobot, diagLengthSquared);
		
	experiment(robot, &currentRobot, &lastTime, &lights, serialPort);
		
	report(robot, &position, serialPort);
		
	for (int i=0; i<N_ROBOTS; i++)
		oldrobot[i] = robot[i];
	
	visualize(bitmap, data,
			  bytesPerRow, bytesPerPixel,
			  imgSize.width, imgSize.height,
			  imageView);
	
	NSMutableArray *robots = [NSMutableArray array];
	NSMutableArray *goals = [NSMutableArray array];
	
	for (int i=0; i<N_ROBOTS; i++) {
		float X, Y;
		X = (robot[i].x/imgSize.width)*2.f - 1.f;
		Y = -((robot[i].y/imgSize.height)*2.f - 1.f) * imgSize.height/imgSize.width;
		if(robot[i].regionMax >= ROBOT_THRESH) {
			[robots addObject:[NSDictionary dictionaryWithObjectsAndKeys:
							   [NSNumber numberWithFloat:X], @"X",
							   [NSNumber numberWithFloat:Y], @"Y",
							   [NSNumber numberWithFloat:robot[i].theta*180.f/M_PI], @"Theta",
							   [NSString stringWithFormat:@"Robot %d%@", robot[i].id, robot[i].light?@"+":@""], @"Label", nil]];
			
		}
	}

	[qcView setValue:[NSDictionary dictionaryWithObjectsAndKeys:
					  robots, @"Robots",
					  goals, @"Goals",
					  nil] forInputKey:@"Structure"];
}

- (void)windowWillClose:(NSNotification *)notification {
	[NSApp terminate:self];
}

@end
