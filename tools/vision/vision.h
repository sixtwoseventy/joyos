/*
 *  vision.h
 *  vision
 *
 *  Created by 6. 270 on 1/17/10.
 *  Copyright 2010 Massachusetts Institute of Technology. All rights reserved.
 *
 */

#define R 60

typedef struct {
	uint8_t id;
	float x, y, theta, sum, moment;
	bool light;
	int xmax, ymax, thresh;
} sighting;

#define ROBOT_PIXEL_THRESH 6
#define ROBOT_THRESH 3.5

void locate(unsigned char *data, int bytesPerRow, int bytesPerPixel, int width, int height,
			int *xout, int *yout);
void erase(unsigned char *data, unsigned char *mask, int bytesPerRow, int bytesPerPixel, int width, int height, int x, int y, int dx, int dy);
void align(unsigned char *data, unsigned char *mask, int bytesPerRow, int bytesPerPixel, int width, int height,
			int x, int y, float *xout, float *yout, float *thetaout, bool *light, int *thresh, float *momentout, float *sumout);
void findRobot(unsigned char *data, int bytesPerRow, int bytesPerPixel, int width, int height, sighting *robot);

#define SWAP(_x_, _y_) {__typeof__(_x_) _z_; _z_=_x_; _x_=_y_; _y_=_z_;}
