#include <board.h>
#include <kern/global.h>
#include <kern/thread.h>
#include <kern/lock.h>
#include <avr/interrupt.h>
#include <math.h>
#include <gyro.h>
#include <board.h>
//#include <pid.h>

#define GYRO_PORT 11
//#define MEASURE_SAMPLES 1400
#define NUM_SAMPLES 100

uint16_t samples[NUM_SAMPLES];
uint32_t time[NUM_SAMPLES];

void grab_samples (void) {
	for (uint32_t i = 0; i < NUM_SAMPLES; i++) {
		samples[i] = analog_read (GYRO_PORT);
		time[i] = get_time ();
	}
}

void outp_samples (void) {
	uart_printf ("sampes = [");
	for (uint32_t i = 0; i < NUM_SAMPLES-1; i++) {
		uart_printf ("%u,",samples[i]);
	}
	uart_printf ("%u]\n",samples[NUM_SAMPLES-1]);

	uart_printf ("time = [");
	for (uint32_t i = 0; i < NUM_SAMPLES; i++) {
		uart_printf ("%lu,",time[i]);
	}
	uart_printf ("%lu]\n",time[NUM_SAMPLES-1]);
}
 
int usetup (void) {
	set_auto_halt (0);
	return 0;
}

int umain(void) {
	cli();
	printf ("\nready");

	do {
		go_click ();

		printf ("\nsampling...");
		grab_samples ();

		printf ("\noutputing...\n");
		outp_samples ();

		printf ("\ndone. ready");

	} while (1);

	return 0;
}

