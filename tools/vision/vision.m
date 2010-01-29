/*
 *  vision.c
 *  vision
 *
 *  Created by 6. 270 on 1/17/10.
 *  Copyright 2010 Massachusetts Institute of Technology. All rights reserved.
 *
 */

#import "vision.h"
#import "seedfill.h"
#import "AppController.h"

#define ROI_LEFT 70
#define ROI_RIGHT 110
#define ROI_TOP 95
#define ROI_BOTTOM 20

// given a bitmap, obtain a location hint
void locate(unsigned char *data, int bytesPerRow, int bytesPerPixel, int width, int height,
			int *xout, int *yout) {
	int max = 0;
	int xmax = 0;
	int ymax = 0;
	
	for (int y=ROI_TOP; y<height-ROI_BOTTOM; y++) {
		for (int x=ROI_LEFT; x<width-ROI_RIGHT; x++) {
			unsigned char value = data[bytesPerPixel * x + bytesPerRow * y];
			if (value > max) {
				max = value;
				xmax = x;
				ymax = y;
			}
		}
	}
	
	*xout = xmax;
	*yout = ymax;

}

// given a bitmap, blank the masked area in a window
void blank(unsigned char *data, int bytesPerRow, int bytesPerPixel, int width, int height, int x, int y, int dx, int dy) {
	for (int yy=MAX(y,0); yy<MIN(y+dy, height); yy++) {
		for (int xx=MAX(x,0); xx<MIN(x+dx, width); xx++) {
			data[bytesPerPixel * xx + bytesPerRow * yy] = 0;
		}
	}
}

void erase(unsigned char *data, unsigned char *mask, int bytesPerRow, int bytesPerPixel, int width, int height, int x, int y, int dx, int dy) {
	for (int yy=MAX(y,0); yy<MIN(y+dy, height); yy++) {
		for (int xx=MAX(x,0); xx<MIN(x+dx, width); xx++) {
			if (mask[xx*bytesPerPixel + yy*bytesPerRow]) {
				data[xx*bytesPerPixel + yy*bytesPerRow] = 0;
			}
		}
	}
}

void findcentroid(Window *win, unsigned char *data, unsigned char *mask, int bytesPerRow, int bytesPerPixel,
				  float centroid[2], float *sum, float *maxOut) {
	float norm = 0.f;
	float max = 0.f;
	centroid[0] = 0.f;
	centroid[1] = 0.f;
	for (int y=win->y0; y<=win->y1; y++) {
		for (int x=win->x0; x<=win->x1; x++) {
			if (mask[x*bytesPerPixel + y*bytesPerRow]) {
				float value = data[x*bytesPerPixel + y*bytesPerRow] / 255.f;
				centroid[0] += x*value;
				centroid[1] += y*value;
				norm += value;
				max = max < value ? value : max;
			}
		}
	}
	centroid[0] /= norm;
	centroid[1] /= norm;		
	*sum = norm;
	*maxOut = max;
}

float angle(Window *win, unsigned char *data, unsigned char *mask, int bytesPerRow, int bytesPerPixel,
			float centroid[2]) {
	float xsum = 0.f, ysum = 0.f;
	for (int y=win->y0; y<=win->y1; y++) {
		for (int x=win->x0; x<=win->x1; x++) {
			if (mask[x*bytesPerPixel + y*bytesPerRow]) {
				float mag = sqrt((x-centroid[0])*(x-centroid[0]) + (y-centroid[1])*(y-centroid[1]));
				float value = data[x*bytesPerPixel + y*bytesPerRow] / 255.f;
				xsum += (x-centroid[0])*value/mag;
				ysum += (y-centroid[1])*value/mag;
			}
		}
	}
	return atan2(xsum, ysum);
}

void hist(Window *win, unsigned char *data, unsigned int *h, unsigned int *total, int bytesPerRow, int bytesPerPixel) {
	for (int i=0; i<256; i++)
		h[i] = 0;
	for (int y=win->y0; y<=win->y1; y++) {
		for (int x=win->x0; x<=win->x1; x++) {
			h[data[x*bytesPerPixel + y*bytesPerRow]]++;
		}
	}
	*total = (win->y1 - win->y0 + 1) * (win->x1 - win->x0 + 1);
}

// given a bitmap and a location hint, determine a better location estimate
// and an angle estimate; furthermore, generate a mask
void align(unsigned char *data, unsigned char *mask, int bytesPerRow, int bytesPerPixel, int width, int height,
		   int x, int y, float *xout, float *yout, float *thetaout, bool *light, int *thresh, float *momentout, float *sumout) {
	// clear the mask
	blank(mask, bytesPerRow, bytesPerPixel, width, height, x-R, y-R, 2*R+1, 2*R+1);
	// choose a work region
	Window win = {MAX(x-R, 0), MAX(y-R, 0), MIN(x+R, width-1), MIN(y+R, height-1)};
	// choose a threshold by entropy maximization
	unsigned int h[256], hc[256], ht, running = 0;
	float moment, max;
	int threshold = 0;
	int i=0;
	hist(&win, data, h, &ht, bytesPerRow, bytesPerPixel);
	
	for (i=5; i<256; i++) {
		moment += h[i]*pow(i/255.,2.0);
	}
	
	for (i=0; i<256; i++) {
		hc[i] = running;
		running += h[i];
	}
	for (i=0; i<256; i++) {
		// p is the probability that a randomly chosen pixel has a value >= i
		float p = 1 - (((float)hc[i]) / ht);
		// p should be about 375/ht
//		if (p < (375.f / ht) * 1.5) {
		if (p < (750.f / ht) * 1.5) {
			threshold = i;
			break;
		}
	}

	//	printf("Threshold %d\n", threshold);
	//	threshold = data[x*bytesPerPixel + y*bytesPerRow]*2/4;
	//	printf("At chosen threshold, p is %.2f rather than %.2f\n", 1 - (((float)h[threshold-1]) / ht), 375.f / ht);
	
	//	fill(x, y, threshold2, &win, data, mask, bytesPerRow, bytesPerPixel);
	// flood fill area above threshold to generate mask (keep bounds)
	//data[x*bytesPerPixel + y*bytesPerRow] * 2 / 4
	threshold = ROBOT_PIXEL_THRESH;
	int total=0;
	fill(x, y, threshold, &win, data, mask, bytesPerRow, bytesPerPixel,&total);
	// within bounds, for masked pixels,
	//   find centroid
	float centroid[2];
	float sum;
	findcentroid(&win, data, mask, bytesPerRow, bytesPerPixel, centroid, &sum, &max);
	moment /= sum;
	
	// Now look for pixels that stand out against the background
	if (total != 0) *light = max - (sum/((float)total)) > .12;
	else *light = 0;
		
	////	 estimate angle from difference between centroid and center
	//float angle1 = -atan2((win.x1+win.x0)*.5f - centroid[0], (win.y1+win.y0)*.5f - centroid[1]);
	//	 recenter to centroid
	//   integrate angle phasor
	float angle1 = angle(&win, data, mask, bytesPerRow, bytesPerPixel, centroid);
	//   recenter to tail
	//   integrate angle phasor
	//   recenter to head
	//   integrate angle phasor
	//   take a weighted average
	*xout = centroid[0];
	*yout = centroid[1];
	*thetaout = angle1;
	*thresh = threshold;
	*momentout = moment;
	*sumout = sum;
}

void findRobot(unsigned char *data, int bytesPerRow, int bytesPerPixel, int width, int height, sighting *robot) {
	locate(data+1, bytesPerRow, bytesPerPixel, width, height,
						 &robot->xmax, &robot->ymax);
	
	align(data+1, data+0, bytesPerRow, bytesPerPixel,
		  width, height,
		  robot->xmax, robot->ymax,
		  &robot->x, &robot->y, &robot->theta, &robot->light, &robot->thresh, &robot->moment, &robot->sum);
	
	erase(data+1, data+0, bytesPerRow, bytesPerPixel, width, height,
		  robot->xmax-R, robot->ymax-R, 2*R+1, 2*R+1);
}

#define SORT(_x_, _N_, _f_) \
{	for (int i=0; i<_N_-1; i++) \
for (int j=i+1; j<_N_; j++) \
if (_f_(_x_[i], _x_[j])) \
SWAP(_x_[i], _x_[j]); }

bool by_confidence(sighting a, sighting b) {
	return a.thresh < b.thresh;
}

void sort_by_confidence(sighting x[], int N) {
	SORT(x, N, by_confidence);
}
