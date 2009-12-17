// Include headers from OS
#include <config.h>
#include <hal/delay.h>
#include <rf.h>
#include <string.h> //memcpy
#include <nrf24l01.h>

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)(~(1 << mask)))

#include <avr/interrupt.h>
#include <hal/uart.h>

typedef enum { //Types of packets
	POSITION = 0x00, //Updated position of this bot, other bot, or mouse
	START = 0x01, //Start of the round
	STOP = 0x02, //End of the round
	STRING = 0x03, //String from bot to board
	SYNC = 0x04 //Not sure how to use yet.  Established IDs of all bots/vision system on board
} packet_type;


ISR (RF_INTERRUPT)//RF Interrupt
{
	cli();//Disable Interrupts
	rx_data_nRF24L01(); //Read data
	uint8_t packet_type_id = rf_rx_array[0];
	switch (packet_type_id) {
		case POSITION:
			; //GCC compiler quirk, label statement can't be followed by a declaration, so here's an empty statement
			//Whose position is updated?
			uint8_t target = rf_rx_array[1]; 
			float * target_position;
			if (target == self_id) 			target_position = self_position;
			else if (target == other_id) 	target_position = other_position;
			else if (target == mouse_id) 	target_position = mouse_position;
			else break;
			//Copy the payload to the corresponding position
			memcpy(target_position,&rf_rx_array[2],2*sizeof(float));
			break;
		default:
			break;
	}
	//uart_printf("\n(%f, %f)",self_position[0],self_position[1]);
	sei();//Enable Interrupts
}

void transmit_position_packet(uint8_t id, float x, float y) {
	//Byte 0: Packet type
	//Byte 1: ID of robot at position
	//Byte [2:6]: X coord, float
	//Byte [6:10]: Y coord, float
	config_tx_nRF24L01(); //Enter transmit mode
	rf_tx_array[0] = POSITION;
	rf_tx_array[1] = id;
	memcpy(&rf_tx_array[2],&x,sizeof(float));
	memcpy(&rf_tx_array[6],&y,sizeof(float));
	tx_data_nRF24L01(); //Transmit data_array
	config_rx_nRF24L01(); //Return to receive mode
}

// Initialize RF
void rf_init (void) {
	EIMSK |= 1<<INT7;//Enable RF interrupt
	
	master_id = 0; //ID of the master (vision system)
	self_id = 1; //ID of this robot
	other_id = 2; //ID of the other robot
	mouse_id = 3; //ID of the mouse

	self_position[0] = 0.0;
	self_position[1] = 0.0;
	other_position[0] = 0.0;
	other_position[1] = 0.0;
	mouse_position[0] = 0.0;
	mouse_position[1] = 0.0;
	
	delay_busy_ms(1000);
	
	init_24L01_pins();

	delay_busy_ms(1000);
	
	config_rx_nRF24L01(); //Enable receive mode
}


float get_self_position_x() { return self_position[0]; }

float get_self_position_y() { return self_position[1]; }

float get_other_position_x() { return other_position[0]; }

float get_other_position_y() { return other_position[1]; }

float get_mouse_position_x() { return mouse_position[0]; }

float get_mouse_position_y() { return mouse_position[1]; }