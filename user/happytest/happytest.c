/*
 * The MIT License
 *
 * Copyright (c) 2007 MIT 6.270 Robotics Competition
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <global.h>
#include <board.h>
#include <thread.h>

/**
 * Display testName and 'Press Go'
 */
void startTest(char testName[]) {
	printf("\n%sPress Go",testName);
	goClick();
}

/**
 * Test Servos
 * servos 0 - 5 positions are set by frob knob
 */
void testServos() {
	uint8_t srv;
	uint16_t pos;
	while (!stopPress()) {
		pos = readFrob()/2;
		printf("\nservos=%d",pos);
		for (srv=0;srv<6;srv++)
			servoSetPos(srv,pos);
		pause(50);
	}
}

/**
 * Test Motors
 * Motor velocity is set by frob knob
 * Cycle through motors with go button
 */
void testMotors() {
	uint8_t mot=0;
	uint16_t pos;

	while (!stopPress()) {
		pos = readFrob()/2;
		printf("\nmotor%d=%3d %dmA",mot,pos,motorGetCurrentMA(mot));
		motorSetVel(mot,pos-256);
		if (goPress()) {
			goClick();
			motorSetVel(mot,0);
			mot++;
			if (mot==6) mot = 0;
		}
		pause(50);
	}
	motorSetVel(mot,0);
}

/**
 * Test Encoders
 * Displays all encoder counts
 */
void testEncoders() {
	while (!stopPress()) {
		uint16_t e24 = encoderRead(24);
		uint16_t e25 = encoderRead(25);
		uint16_t e26 = encoderRead(26);
		uint16_t e27 = encoderRead(27);
		
		printf("\ne24=%03d e25=%03d e26=%03d e27=%03d",e24,e25,e26,e27);
		pause(50);
	}
}


/**
 * Test Analog Inputs
 * Display single analog input, select with frob knob
 */
void testAnalog() {
	uint8_t port;
	while (!stopPress()) {
		port = (readFrob()/64) + 8;
		printf("\nanalog%02d=%d",port,analogRead(port));
		pause(50);
	}
}

/**
 * Test Digital Inputs
 * Displayed as single 8bit binary num
 */
void testDigital() {
	while (!stopPress()) {
		printf("\ndigital=");
		for (uint8_t i=0;i<8;i++)
			lcdPrintChar(digitalGet(i) ? '1' : '0',NULL);
		pause(50);
	}
}

/**
 * Test RF
 * Display X, Y coordinates of board
 */
void testRF() {
	while (!stopPress()) {
		uint8_t team = readFrob()>511;
		printf("\nrobot %d         x:%d y:%d",team,rf_get_x(team),rf_get_y(team));
		pause(50);
	}
}

// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
	return 0;
}

/**
 * Run all tests
 */
int
umain (void) {
	startTest("Happytest v0.51 ");

	startTest("Servo Test      ");
	testServos();
	
	startTest("Motor Test      ");
	testMotors();

	startTest("Digital Test    ");
	testDigital();

	startTest("Analog Test     ");
	testAnalog();
	
	startTest("Encoder Test    ");
	testEncoders();
	
	startTest("RF Test         ");
	testRF();

	printf("\nTests complete.");
	while (1);
	return 0;
}

