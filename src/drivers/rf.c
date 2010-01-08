// Include headers from OS
#include <config.h>
#include <hal/delay.h>
#include <rf.h>
#include <string.h> //memcpy
#include <nrf24l01.h>
#include <kern/global.h>

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)(~(1 << mask)))

#include <avr/interrupt.h>
#include <hal/uart.h>

ISR (RF_INTERRUPT)//RF Interrupt
{
	cli();//Disable Interrupts
	rx_data_nRF24L01(); //Read data

	uint8_t type = rx.type;
	uint8_t address = rx.address;

	switch (type) {
		case POSITION:
			//Whose position is updated?

			objects[0] = rx.payload.coords[0];
			objects[1] = rx.payload.coords[1];
			objects[2] = rx.payload.coords[2];
			objects[3] = rx.payload.coords[3];

			break;
		case GOAL:

			goal_position = rx.payload.coords[0];

			break;

/*		case START:
	
			//Remaining bytes are robots which are starting.  Check if we're one of them.
			for (uint8_t i = 0; i < 30; i++) {
				target = rx.packet_buf.payload[i];
				if (target == self_id) { round_start(); break; }
			}
			break;
		case STOP:
			//Remaining bytes are robots which are stopping.  Check if we're one of them.
			for (uint8_t i = 0; i < 30; i++) {
				target = rx.packet_buf.payload[i];
				if (target == self_id) { round_end(); break; }
			}
			break;

		case STRING:
			printf("%s",rx.packet_buf.payload);
			break;*/
		default:
			break;
	}
	//printf("\n(%f, %f)",self_position[0],self_position[1]);
	sei();//Enable Interrupts
}

void transmit_raw_packet(uint8_t * buf, uint8_t bytes){
	config_tx_nRF24L01(); //Enter transmit mode
	memcpy((unsigned char*)&tx,buf,bytes); // Copy buffer
	tx_data_nRF24L01(); //Transmit data_array
	config_rx_nRF24L01(); //Return to receive mode
}

// Initialize RF
void rf_init (void) {
	EIMSK |= 1<<INT7;//Enable RF interrupt
	
	delay_busy_ms(5);
	
	init_24L01_pins();

	delay_busy_ms(5);
	
	config_rx_nRF24L01(); //Enable receive mode
}

