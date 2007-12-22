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

#include <board.h>

/**
 * Display testName and 'Press Go'
 */
void start_test(char testName[]) {
	printf("\n%sPress Go",testName);
	go_click();
}

/**
 * Test Servos
 * servos 0 - 5 positions are set by frob knob
 */
void test_servos() {
	uint8_t srv;
	uint16_t pos;
	while (!stop_press()) {
		pos = frob_read()/2;
		printf("\nservos=%d",pos);
		for (srv=0;srv<6;srv++)
			servo_set_pos(srv,pos);
		pause(50);
	}
}

/**
 * Test Motors
 * Motor velocity is set by frob knob
 * Cycle through motors with go button
 */
void test_motors() {
	uint8_t mot=0;
	uint16_t pos;

	while (!stop_press()) {
		pos = frob_read()/2;
		printf("\nmotor%d=%3d %dmA",mot,pos,motor_get_current_MA(mot));
		motor_set_vel(mot,pos-256);
		if (go_press()) {
			go_click();
			motor_set_vel(mot,0);
			mot++;
			if (mot==6) mot = 0;
		}
		pause(50);
	}
	motor_set_vel(mot,0);
}

/**
 * Test Encoders
 * Displays all encoder counts
 */
void test_encoders() {
	while (!stop_press()) {
		uint16_t e24 = encoder_read(24);
		uint16_t e25 = encoder_read(25);
		uint16_t e26 = encoder_read(26);
		uint16_t e27 = encoder_read(27);
		
		printf("\ne24=%03d e25=%03d e26=%03d e27=%03d",e24,e25,e26,e27);
		pause(50);
	}
}


/**
 * Test Analog Inputs
 * Display single analog input, select with frob knob
 */
void test_analog() {
	uint8_t port;
	while (!stop_press()) {
		port = (frob_read()/64) + 8;
		printf("\nanalog%02d=%d",port,analog_read(port));
		pause(50);
	}
}

/**
 * Test Digital Inputs
 * Displayed as single 8bit binary num
 */
void test_digital() {
	while (!stop_press()) {
		printf("\ndigital=");
		for (uint8_t i=0;i<8;i++)
			lcd_print_char(digital_read(i) ? '1' : '0',NULL);
		pause(50);
	}
}

/**
 * Test RF
 * Display X, Y coordinates of board
 */
void test_rf() {
	while (!stop_press()) {
		uint8_t team = frob_read()>511;
		printf("\nrobot %d         x:%d y:%d",team,rf_get_x(team),rf_get_y(team));
		pause(50);
	}
}

// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
	set_auto_halt(0);
	return 0;
}

/**
 * Run all tests
 */
int
umain (void) {
	start_test("Happytest v0.61 ");

	start_test("Servo Test      ");
	test_servos();
	
	start_test("Motor Test      ");
	test_motors();

	start_test("Digital Test    ");
	test_digital();

	start_test("Analog Test     ");
	test_analog();
	
	start_test("Encoder Test    ");
	test_encoders();
	
	start_test("RF Test         ");
	test_rf();

	printf("\nTests complete.");
	while (1);
	return 0;
}

