#include <kern/global.h>
#include <board.h>
#include <kern/thread.h>
#include <math.h>
#include <gyro.h>
#include <board.h>
//#include <pid.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define GYRO_PORT 11

int display_angle (void) {
	for (;;) {
		printf("\ntheta = %.2f", gyro_get_degrees());
		//printf("\nreading = %.2f", gyro_read());
		pause(500);
	}

	return 0;
}
 
// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
	printf("\nPlace robot,    press go.");
	go_click();
	printf("\nStabilizing...");
	pause(500); /* Wait for robot to stabilize mechanically */
	printf("\nCalibrating     offset...");
	//gyro_init(GYRO_PORT, 496.2, 29);
	gyro_init(GYRO_PORT, 496.2, 29);
	printf("\nDone calibration");

	set_auto_halt (0);
	return 0;
}

void set_turn (float value) {
	printf("\nActuate %f", (double) value);
}

int umain(void) {
	create_thread(&display_angle, 64, 0, "display angle");

	extern uint16_t *_samples;
	extern uint16_t _num_samples;
	uint16_t *samp = (uint16_t *) malloc (_num_samples * sizeof (uint16_t));

	while (1) {
		uart_printf ("heartbeat\n");

		cli();
		for (uint16_t i = 0; i < _num_samples; i++) {
			samp[i] = _samples[i];
		}
		sei();

		uart_printf ("samples = [");
		for (uint16_t i = 0; i < _num_samples-1; i++) {
			uart_printf ("%u,", samp[i]);
		}
		uart_printf ("%u];\n", samp[_num_samples-1]);
		uart_printf ("  read = %.2f\n", gyro_read());
//		pause(500);

		analog_read(0);


		servo_set_pos (0,10);
		pause (1000);
		servo_set_pos (0,500);
		pause (1000);
	}

	return 0;
}

