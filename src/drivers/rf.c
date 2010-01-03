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

typedef enum { //Types of packets
	POSITION = 0x00, //Updated position of this bot, other bot, or mouse
	START = 0x01, //Start of the round
	STOP = 0x02, //End of the round
	STRING = 0x03, //String from bot to board
	SYNC = 0x04, //Not sure how to use yet.  Established IDs of all bots/vision system on board
	GOAL = 0x05 //Sets the target position of this robot
} packet_type;


ISR (RF_INTERRUPT)//RF Interrupt
{
	cli();//Disable Interrupts
	rx_data_nRF24L01(); //Read data
	uint8_t packet_type_id = rf_rx_array[0];
	uint8_t target;
	switch (packet_type_id) {
		case POSITION:
			//Whose position is updated?
			target = rf_rx_array[1]; 
			float * target_position;
			if (target == self_id) 			target_position = self_position;
			else if (target == other_id) 	target_position = other_position;
			else if (target == mouse_id) 	target_position = mouse_position;
			else break;
			//Copy the payload to the corresponding position
			memcpy(target_position,&rf_rx_array[2],2*sizeof(float));
			break;
		case GOAL:
			//Whose position is updated?
			target = rf_rx_array[1];
			//Copy the payload to the corresponding position
			memcpy(goal_position,&rf_rx_array[2],2*sizeof(float));
			break;
		case START:
			//Remaining bytes are robots which are starting.  Check if we're one of them.
			for (uint8_t i = 1; i < PAYLOAD_SIZE; i++) {
				target = rf_rx_array[i];
				if (target == self_id) { round_start(); break; }
			}
			break;
		case STOP:
			//Remaining bytes are robots which are stopping.  Check if we're one of them.
			for (uint8_t i = 1; i < PAYLOAD_SIZE; i++) {
				target = rf_rx_array[i];
				if (target == self_id) { round_end(); break; }
			}
			break;
		default:
			break;
	}
	//printf("\n(%f, %f)",self_position[0],self_position[1]);
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

void transmit_goal_packet(uint8_t id, float x, float y) {
	//Byte 0: Packet type
	//Byte 1: ID of robot at position
	//Byte [2:6]: X coord, float
	//Byte [6:10]: Y coord, float
	config_tx_nRF24L01(); //Enter transmit mode
	rf_tx_array[0] = GOAL;
	rf_tx_array[1] = id;
	memcpy(&rf_tx_array[2],&x,sizeof(float));
	memcpy(&rf_tx_array[6],&y,sizeof(float));
	tx_data_nRF24L01(); //Transmit data_array
	config_rx_nRF24L01(); //Return to receive mode
}

void transmit_start_packet(uint8_t num_bots, uint8_t bot_ids[]) {
	//Byte 0: Packet type
	//Byte [1:10]: IDs of robots to start
	config_tx_nRF24L01(); //Enter transmit mode
	uint8_t start_index = 0;
	uint8_t end_index = start_index + PAYLOAD_SIZE-1;
	//Maybe you've got a ton of robots to activate?
	while (num_bots > end_index) {
		rf_tx_array[0] = START;
		for (uint8_t i = 0; i < PAYLOAD_SIZE-1; i++) {
			rf_tx_array[i+1] = bot_ids[start_index + i];
		}
		tx_data_nRF24L01();
		start_index += PAYLOAD_SIZE-1;
		end_index += PAYLOAD_SIZE-1;
	}
	//We're on the last packet
	rf_tx_array[0] = START;
	for (uint8_t i = 0; i < PAYLOAD_SIZE-1; i++) {
		if (start_index + i < num_bots) {
			rf_tx_array[i+1] = bot_ids[start_index + i];
		} else {
			//Fill the remaining byte entries with zero
			rf_tx_array[i+1] = 0;
		}
	}
	tx_data_nRF24L01(); //Transmit data_array
	config_rx_nRF24L01(); //Return to receive mode
}

void transmit_stop_packet(uint8_t num_bots, uint8_t bot_ids[]) {
	//Byte 0: Packet type
	//Byte [1:10]: IDs of robots to stop
	config_tx_nRF24L01(); //Enter transmit mode
	uint8_t start_index = 0;
	uint8_t end_index = start_index + PAYLOAD_SIZE-1;
	//Maybe you've got a ton of robots to activate?
	while (num_bots > end_index) {
		rf_tx_array[0] = STOP;
		for (uint8_t i = 0; i < PAYLOAD_SIZE-1; i++) {
			rf_tx_array[i+1] = bot_ids[start_index + i];
		}
		tx_data_nRF24L01();
		start_index += PAYLOAD_SIZE-1;
		end_index += PAYLOAD_SIZE-1;
	}
	//We're on the last packet
	rf_tx_array[0] = STOP;
	for (uint8_t i = 0; i < PAYLOAD_SIZE-1; i++) {
		if (start_index + i < num_bots) {
			rf_tx_array[i+1] = bot_ids[start_index + i];
		} else {
			//Fill the remaining byte entries with zero
			rf_tx_array[i+1] = 0;
		}
	}
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

	//Initialize positions
	self_position[0] = 0.0;
	self_position[1] = 0.0;
	other_position[0] = 0.0;
	other_position[1] = 0.0;
	mouse_position[0] = 0.0;
	mouse_position[1] = 0.0;
	
	//Initialize goal position
	goal_position[0] = 0.0;
	goal_position[1] = 0.0;
	goal_id = 0;
	
	delay_busy_ms(5);
	
	init_24L01_pins();

	delay_busy_ms(5);
	
	config_rx_nRF24L01(); //Enable receive mode
}


float get_self_position_x() { return self_position[0]; }

float get_self_position_y() { return self_position[1]; }

float get_other_position_x() { return other_position[0]; }

float get_other_position_y() { return other_position[1]; }

float get_mouse_position_x() { return mouse_position[0]; }

float get_mouse_position_y() { return mouse_position[1]; }
