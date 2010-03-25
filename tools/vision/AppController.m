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

// reset -> accept team numbers, count down from 60 seconds, uncheck box
// check box -> force calibration period to continue, or uncheck to immediately start

@implementation AppController

- (void) gotDataNotification:(NSNotification *)notification {
	fflush(stdout);
	NSData *data = [[notification userInfo] objectForKey:NSFileHandleNotificationDataItem];
	[(NSFileHandle*)[NSFileHandle fileHandleWithStandardOutput] writeData:data];
	[serialPort readInBackgroundAndNotify];
}

- (IBAction) fixCameraMode:(id)sender
{
	/*videoDevice = [[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo] objectAtIndex:0];
	[videoDevice open:nil];
	
	if( !videoDevice ) {
		NSLog( @"No video input device" );
		exit( 1 );
	}
	
	videoInput = [[QTCaptureDeviceInput alloc] initWithDevice:videoDevice];
	
	captureSession = [[QTCaptureSession alloc] init];
	[captureSession addInput:videoInput error:nil];
//	[captureSession startRunning];
	
	captureView = [[QTCaptureView alloc] init];
	[captureView setCaptureSession:captureSession];
	[captureView setVideoPreviewConnection:[[captureView availableVideoPreviewConnections] objectAtIndex:0]];
	
	
	// Setting a lower resolution for the CaptureOutput here, since otherwise QTCaptureView
	// pulls full-res frames from the camera, which is slow. This is just for cosmetics.
	NSDictionary * pixelBufferAttr = [NSDictionary dictionaryWithObjectsAndKeys:
									  [NSNumber numberWithInt:640], kCVPixelBufferWidthKey,
									  [NSNumber numberWithInt:480], kCVPixelBufferHeightKey, nil];
	[[[captureSession outputs] objectAtIndex:0] setPixelBufferAttributes:pixelBufferAttr];
//	[captureSession stopRunning];

	[captureSession release];*/
	
	// Ok, this might be all kinds of wrong, but it was the only way I found to map a
	// QTCaptureDevice to a IOKit USB Device. The uniqueID method seems to always(?) return
	// the locationID as a HEX string in the first few chars, but the format of this string
	// is not documented anywhere and (knowing Apple) might change sooner or later.
	//
	// In most cases you'd be probably better of using the UVCCameraControls
	// - (id)initWithVendorID:(long) productID:(long)
	// method instead. I.e. for the Logitech QuickCam9000:
	// cameraControl = [[UVCCameraControl alloc] initWithVendorID:0x046d productID:0x0990];
	//
	// You can use USB Prober (should be in /Developer/Applications/Utilities/USB Prober.app)
	// to find the values of your camera.
	
//	UInt32 locationID = 0;
//	sscanf( [[videoDevice uniqueID] UTF8String], "0x%8x", &locationID );
//	cameraControl = [[UVCCameraControl alloc] initWithVendorID:0x046d productID:0x0809];
//	[cameraControl setGain:10000.];
	
	//[cameraControl setAutoExposure:NO];
	//[cameraControl setAutoWhiteBalance:NO];
	//[cameraControl setExposure:100.0];
}

- (void) awakeFromNib
{
	NSMutableDictionary *composition = [NSMutableDictionary dictionaryWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"Vision" ofType:@"qtz"]];

	NSMutableDictionary *rootPatch = [[composition objectForKey:@"rootPatch"] mutableCopy];
	[composition setObject:rootPatch forKey:@"rootPatch"];
	
	NSMutableDictionary *state = [[rootPatch objectForKey:@"state"] mutableCopy];
	[rootPatch setObject:state forKey:@"state"];
	
	NSMutableArray *nodes = [[state objectForKey:@"nodes"] mutableCopy];
	[state setObject:nodes forKey:@"nodes"];
	
	int i;
	for (i=0; i<[nodes count]; i++) {
		if ([[[nodes objectAtIndex:i] objectForKey:@"class"] isEqualToString:@"QCVideoInput"]) {
			// this is our video input patch
			NSMutableDictionary *qcvi = [[nodes objectAtIndex:i] mutableCopy];
			[nodes replaceObjectAtIndex:i withObject:qcvi];
			
			NSMutableDictionary *qcvistate = [[qcvi objectForKey:@"state"] mutableCopy];
			[qcvi setObject:qcvistate forKey:@"state"];
			
			[qcvistate setValue:[NSNumber numberWithInt:640] forKey:@"width"]; // Try these with normal values again
			[qcvistate setValue:[NSNumber numberWithInt:480] forKey:@"height"];
			[qcvistate setValue:[NSNumber numberWithBool:NO] forKey:@"highQuality"]; // Does this work?
			
			[qcvistate release];
			[qcvi release];
			break;
		}
	}
	
	[nodes release];
	[state release];
	[rootPatch release];
	
	NSData *xmlData;
	NSString *error;
	
	NSLog(@"%@", [[[[[[composition objectForKey:@"rootPatch"]
					  objectForKey:@"state"]
					 objectForKey:@"nodes"]
					objectAtIndex:i]
				   objectForKey:@"state"]
				  objectForKey:@"width"]);
	xmlData = [NSPropertyListSerialization dataFromPropertyList:composition
														 format:NSPropertyListBinaryFormat_v1_0
											   errorDescription:&error];
	if(xmlData) {
		if(![qcView loadComposition:[QCComposition compositionWithData:xmlData]]) {
			NSLog(@"Could not load composition");
		}
	}
	else {
		NSLog(@"%@", error);
		[error release];
	}
	
	serialPort = open_serial(@"/dev/tty.usbserial-A800cBag");

	tickThread = nil;
	tickLock = [[NSLock alloc] init];
	[self reset:nil];
	
	[self performSelectorInBackground:@selector(lightThread:) withObject:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gotDataNotification:) name:NSFileHandleReadCompletionNotification object:nil];
}

- (IBAction)extendCalibrationPeriod:(id)sender {
	bool checked = [checkBox state] == NSOnState;
	if (checked) {
		// don't start any time soon
		startTime = [[NSDate date] timeIntervalSince1970] + 1e6;
	} else {
		// start immediately
		startTime = [[NSDate date] timeIntervalSince1970] + 5.f;
	}
}

- (IBAction)reset:(id)sender {
	@synchronized(self) {
		if (![tickLock tryLock]) {
			[self performSelector:@selector(reset:) withObject:nil afterDelay:0.f];
			return;
		}
		
		transmittedStartPacket = 0;
		transmittedStopPacket = 0;
		[checkBox setState:NSOffState];
		startTime = [[NSDate date] timeIntervalSince1970] + 60.f;
		
		if (tickThread) {
			killTickThread = YES;
			while (killTickThread)
				usleep(10000);
			tickThread = nil;
		}
		[tickLock unlock];
		
		[qcView stopRendering];
		[qcView startRendering];
		
		[self fixCameraMode:nil];
		
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
		packet_buffer tx = lights;
		send_packet(serialPort,&tx,sizeof(packet_buffer));
		
		// start robots
		start.type = START;
		for (int i=0; i<N_ROBOTS; i++){
			start.payload.array[i] = position.payload.coords[i].id;
		}
		
		killTickThread = NO;
		[self performSelectorInBackground:@selector(tickThread:) withObject:nil];
	}
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
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
    }
	tickThread = nil;
	killTickThread = NO;
    [pool release];
}

- (void)lightThread:(id)arg {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(light:) userInfo:nil repeats:YES];
	
    while (1) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
    }
    [pool release];
}

- (void)light:(id)sender {
	packet_buffer tx = lights;
	send_packet(serialPort,&tx,sizeof(packet_buffer));
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
}

void reorder(sighting robot[], sighting oldrobot[], float diagLengthSquared) {
	// make an effort to figure out which is which
	// assignments is a map of new robots -> old robots
	int assignments[N_ROBOTS+N_MARGIN];
	float distance[N_ROBOTS+N_MARGIN][N_ROBOTS];
	float confidence[N_ROBOTS+N_MARGIN];
	for (int i=0; i<N_ROBOTS+N_MARGIN; i++) {
		for (int j=0; j<N_ROBOTS; j++) {
			float dx = robot[i].x - oldrobot[j].x;
			float dy = robot[i].y - oldrobot[j].y;
			float dtheta_squared = angleDiff(robot[i].theta, oldrobot[j].theta);
			if (isnan(dx) || isnan(dy))
				dx = dy = sqrt(diagLengthSquared);
			if (isnan(dtheta_squared))
				dtheta_squared = diagLengthSquared;
			
			float oldUnConfSquared = robot[i].sum < ROBOT_THRESH ? diagLengthSquared : 0;
			float newUnConfSquared = robot[i].sum < ROBOT_THRESH ? diagLengthSquared : 0;
			
			distance[i][j] = dx*dx+dy*dy+dtheta_squared*R*R/4.f+oldUnConfSquared+newUnConfSquared;
			assert(!isnan(distance[i][j]));
		}
		assignments[i] = -1;
		confidence[i] = robot[i].sum;
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
/*
	// select the N_ROBOTS most confident entries in robot and match them
	// to oldrobot entries
	for (int i=0;i<N_ROBOTS;i++){
		
		// find the next largest confidence
		float maxConfidence = -INFINITY;
		int maxConfIndex=-1;
		for (int j=0;j<(N_ROBOTS+N_MARGIN);j++){
			if (maxConfidence < confidence[j]){
				maxConfIndex = j;
				maxConfidence = confidence[j];
			}
		}
		// then match the corresponding robot to the nearest entry in
		// oldrobots
		float minDistance=INFINITY;
		int minDistanceIndex=-1;
		for (int j=0;j<N_ROBOTS;j++){
			
			// if this oldrobot has been assigned already, skip it
			if (assignments[i] != -1) continue;
			
			// search for the min distance
			float dx = robot[maxConfIndex].x - oldrobot[j].x;
			float dy = robot[maxConfIndex].y - oldrobot[j].y;
			if (isnan(dx) || isnan(dy))
				dx = dy = sqrt(diagLengthSquared);
			float distance = dx*dx+dy*dy;
			if (distance < minDistance){
				minDistanceIndex = j;
				minDistance = distance;
			}
		}
		
		//printf("%d->%d\n",maxConfIndex,minDistanceIndex);
		assignments[maxConfIndex] = minDistanceIndex;
		// robot[jMax] matched
		// remove this entry from consideration
		confidence[maxConfIndex] = -INFINITY;
	}*/
	
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

void experiment(sighting robot[], int *currentRobot, NSTimeInterval *lastTime, volatile packet_buffer *lights, NSFileHandle *serialPort) {
	// experiment consists of commanding one robot to turn its light on.
	// then we test which 't' got the brightest and swap the robot->t
	// assignment as necessary. evaluate the results of each experiment
	// after each second, then increment currentRobot.
#define LIGHTLESS_INTERVAL 1.0
	NSTimeInterval now = [[NSDate date] timeIntervalSinceReferenceDate];
	/*if ((now-*lastTime)<LIGHTLESS_INTERVAL){
		lights->payload.lights[*currentRobot].value = 0;
		send_packet(serialPort,lights,sizeof(packet_buffer));
		return;
	} else*/
	if ((now-*lastTime)<LIGHTLESS_INTERVAL){
		for (int i=0; i<4; i++)
			lights->payload.lights[i].value = 0;
		lights->payload.lights[*currentRobot].value = 150;
		packet_buffer tx = *lights;
		send_packet(serialPort,&tx,sizeof(packet_buffer));
		return;
	}

	int litRobot=0, litRobots=0;
	for (int i=0; i<N_ROBOTS; i++) {
		if (robot[i].light && robot[i].sum > ROBOT_THRESH) {
			litRobot = i;
			litRobots++;
		}
	}
	
	//if ((now-*lastTime) > 1.0){
		// evaluate results of the experiment
	if (litRobots){
		if (litRobots == 1) {
			printf("\nfound lamp after %f\n", (now-*lastTime));
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
				printf("Found robot index %d (id=%d) while looking for robot index %d (id=%d) -- swapping\n", litRobot, robot[litRobot].id, *currentRobot, robot[*currentRobot].id);
				SWAP(robot[litRobot], robot[*currentRobot]);
			}
			
			printf("[accept experiment]\n");
		} else{
			printf("    [reject experiment: count=%d]\n", litRobots);
		}
		
		// start the next experiment
		lights->payload.lights[*currentRobot].value = 0;
		*currentRobot = (++*currentRobot) % N_ROBOTS;
		*lastTime = now;
	}
	else if ((now-*lastTime) > 2.0+LIGHTLESS_INTERVAL) {
		printf(" Timing out on lamp %d\n", robot[*currentRobot].id);
		// start the next experiment
		lights->payload.lights[*currentRobot].value = 0;
		*currentRobot = (++*currentRobot) % N_ROBOTS;
		*lastTime = now;
	}
	else {
		printf("w%d", robot[*currentRobot].id);
	}

	packet_buffer tx = *lights;
	send_packet(serialPort,&tx,sizeof(packet_buffer));
}

void report(sighting robot[], volatile packet_buffer *position, NSFileHandle *serialPort) {
	//printf("*************\n");
	for (int i=0; i<N_ROBOTS; i++){
		double x = robot[i].x;
		double y = robot[i].y;
		double X =  8.45150883e+00 * x + -4.52579739e-01 * y + -2.62171630e+03;
		double Y = -2.35832583e-01 * x + -8.26902404e+00 * y +  2.45461144e+03;
		double theta = fmod(robot[i].theta+M_PI*3,M_PI*2) - M_PI;
		position->payload.coords[i].x = (int16_t)MIN(MAX((int)X,-2048),2047); //((robot[i].x-320)*(1<<12)/640.);
		position->payload.coords[i].y = (int16_t)MIN(MAX((int)Y,-2048),2047); //-(int16_t)((robot[i].y-240)*(1<<12)/640.);
		position->payload.coords[i].theta = -(int)(theta*(1<<12)/(M_PI*2));
		position->payload.coords[i].confidence = MIN((1<<12)-1, (unsigned int)(75.*robot[i].sum));
		position->payload.coords[i].id = robot[i].id;
		//printf("[x=%d,y=%d,theta=%d]\n",position->payload.coords[i].x,position->payload.coords[i].y,position->payload.coords[i].theta);
	}
	//printf("*************\n");
	
	packet_buffer tx = *position;
	send_packet(serialPort,&tx,sizeof(packet_buffer));
}

- (void)tick:(id)arg {
	NSBitmapImageRep *bitmap = 0;
	if ([tickLock tryLock]) {
		[self performSelectorOnMainThread:@selector(snapTo:) withObject:[NSValue valueWithPointer:&bitmap] waitUntilDone:YES];
		[tickLock unlock];
	} else
		return;
    NSSize imgSize = [bitmap size];
	
	float t = [[NSDate date] timeIntervalSince1970] - startTime;
	if (t < -15)
		[clock setTextColor:[NSColor greenColor]];
	else if (t < -5)
		[clock setTextColor:[NSColor yellowColor]];
	else if (t < 0)
		[clock setTextColor:[NSColor redColor]];
	else if (t < 90)
		[clock setTextColor:[NSColor greenColor]];
	else if (t < 105)
		[clock setTextColor:[NSColor yellowColor]];
	else
		[clock setTextColor:[NSColor redColor]];
	
	if (t > 0 && transmittedStartPacket < 10) {
		packet_buffer tx = start;
		send_packet(serialPort,&tx,sizeof(packet_buffer));
		transmittedStartPacket++;
	}
	
	if (t > 120 && transmittedStopPacket < 10) {
		packet_buffer tx = stop;
		send_packet(serialPort,&tx,sizeof(packet_buffer));
		transmittedStopPacket++;
	}
	
	if (t <= 120) {
		if (t > 0)
			t = 120 - t;
		int minutes = t/60;
		float seconds = fmod(fabs(t), 60.);
		if ([tickLock tryLock]) {
			if (t > -1000) {
				NSString *s = [NSString stringWithFormat:@"%+2d:%06.3f", minutes, seconds];
				[clock performSelectorOnMainThread:@selector(setStringValue:) withObject:s waitUntilDone:YES];
				//NSLog(@"%@", s);
			}
			[tickLock unlock];
		}
	} else {
		if ([tickLock tryLock]) {
			NSString *s = @"+00:00";
			[clock performSelectorOnMainThread:@selector(setStringValue:) withObject:s waitUntilDone:YES];
			//NSLog(@"%@", s);
			[tickLock unlock];
		}
	}
	
	unsigned char *data = [bitmap bitmapData];
	int bytesPerPixel = [bitmap bitsPerPixel] >> 3;
	int bytesPerRow = [bitmap bytesPerRow];

	float diagLengthSquared = ((float)imgSize.width)*((float)imgSize.width)+((float)imgSize.height)*((float)imgSize.height);
	
	sighting robot[N_ROBOTS+N_MARGIN];
	
	//printf("********\n");
	for (int i=0; i<N_ROBOTS+N_MARGIN; i++){
		findRobot(data, bytesPerRow, bytesPerPixel, imgSize.width, imgSize.height, &robot[i]);
		//printf("sum[%d]=%f\n",i,robot[i].sum);
	}
	//printf("********\n");

	reorder(robot, oldrobot, diagLengthSquared);
	
	experiment(robot, &currentRobot, &lastTime, &lights, serialPort);
		
	report(robot, &position, serialPort);
		
	for (int i=0; i<N_ROBOTS; i++)
		oldrobot[i] = robot[i];
	
	/*visualize(bitmap, data,
			  bytesPerRow, bytesPerPixel,
			  imgSize.width, imgSize.height,
	 imageView);*/
	[bitmap release];
	
	NSMutableArray *robots = [NSMutableArray array];
	NSMutableArray *goals = [NSMutableArray array];
	
	for (int i=0; i<N_ROBOTS; i++) {
		float X, Y;
		X = (robot[i].x/imgSize.width)*2.f - 1.f;
		Y = -((robot[i].y/imgSize.height)*2.f - 1.f) * imgSize.height/imgSize.width;
		if(robot[i].sum >= ROBOT_THRESH) {
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
	[cameraControl release];
	[NSApp terminate:self];
}

@end
