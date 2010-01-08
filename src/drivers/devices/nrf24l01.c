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
 
 //TODO: Logic is still bit-banged, should clean up with SPI module

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <kern/thread.h>
#include <kern/global.h>
#include <hal/delay.h>
#include <nrf24l01.h>
#include <rf.h>

#define L01_PORT		PORTB
#define L01_PORT_PIN	PINB
#define L01_PORT_DD	DDRB

#define L01_SCK	1 //Output
#define MOSI	2 //Output
#define MISO 3 //Input

#define L01_CE_PORT	PORTF
#define L01_CE_DD	DDRF

#define L01_CSN_PORT	PORTE
#define L01_CSN_DD	DDRE

#define L01_CE	3 //Output
#define L01_CSN	3 //Output

#define RF_DELAY	5

#define RX_IRQ	7 //Input
#define L01_IRQ_PORT	PORTE
#define L01_IRQ_PIN	PINE

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)(~(1 << mask)))

//Initializes ATMega168 pins
void init_24L01_pins(void)
{
	SPCR &= ~_BV(SPE);
	ADCSRA &= ~_BV(ADEN);

	//1 = Output, 0 = Input
	L01_PORT_DD |= _BV(PB1) | _BV(PB2);
	L01_PORT_DD &= ~_BV(PB3);


	L01_CE_DD |= (1<<L01_CE);

	L01_CSN_DD |= (1<<L01_CSN);


	//Enable pull-up resistors (page 74)
//	L01_PORT = 0b11111111;

	sbi(L01_CSN_PORT, L01_CSN);
	cbi(L01_CE_PORT, L01_CE); //Stand by mode
	
	for (uint8_t x = 0; x < 20; x++)
	{
		cbi(L01_CSN_PORT, L01_CSN); //Select chip
		tx_spi_byte(0xFF);
		sbi(L01_CSN_PORT, L01_CSN); //Deselect chip
		delay_busy_ms(50);
	}
}


//Sends a string out through nRF
void transmit_string(char * string_out)
{
	uint8_t i;
	
	for(i = 0 ; string_out[i] != '\0' ; i++)
	{
		((unsigned char*)&tx)[1] = string_out[i];
		tx_data_nRF24L01();
		delay_busy_ms(5);
	}

	((unsigned char*)&tx)[1] = '\0';
	tx_data_nRF24L01();
	delay_busy_ms(5);
}

//This sends out the data stored in the data_array
//data_array must be setup before calling this function
void tx_data_nRF24L01(void)
{
	send_command(0x27, 0x7E); //Clear any interrupts
	
	send_command(0x20, 0x7A); //Power up and be a transmitter

	send_byte(0xE1); //Clear TX Fifo
	
	tx_send_payload(0xA0, sizeof(packet)); //Clock in 4 byte payload of data_array

    sbi(L01_CE_PORT, L01_CE); //Pulse CE to start transmission
    delay_busy_ms(1);
    cbi(L01_CE_PORT, L01_CE);
}

//Set up nRF24L01 as a transmitter, does not actually send the data,
//(need to call tx_data_nRF24L01() for that)
uint8_t config_tx_nRF24L01(void)
{
    cbi(L01_CE_PORT, L01_CE); //Go into standby mode
	
	send_command(0x20, 0x7C); //16 bit CRC enabled, be a transmitter

	send_command(0x21, 0x00); //Disable auto acknowledge on all pipes

	send_command(0x24, 0x00); //Disable auto-retransmit

	send_command(0x23, 0x03); //Set address width to 5bytes (default, not really needed)

	send_command(0x26, 0x07); //Air data rate 1Mbit, 0dBm, Setup LNA

	send_command(0x25, 0x02); //RF Channel 2 (default, not really needed)
	
	for (int x = 0; x < sizeof(packet); x++) {
		((unsigned char*)&tx)[x] = 0xE7;
	}
	tx_send_payload(0x30, 5); //Set TX address
	
	send_command(0x20, 0x7A); //Power up, be a transmitter

	return(send_byte(0xFF));
}

//Sends a number of bytes of payload
void tx_send_payload(uint8_t cmd, uint8_t bytes)
{
	uint8_t i;

	cbi(L01_CSN_PORT, L01_CSN); //Select chip
	tx_spi_byte(cmd);
	
	for(i = 0 ; i < bytes ; i++){
		tx_spi_byte(((unsigned char*)&tx)[i]);
	}
	sbi(L01_CSN_PORT, L01_CSN); //Deselect chip
}

//Sends command to nRF, returns status byte
uint8_t send_command(uint8_t cmd, uint8_t data)
{
	uint8_t status;

	cbi(L01_CSN_PORT, L01_CSN); //Select chip
	tx_spi_byte(cmd);
	status = tx_spi_byte(data);
	sbi(L01_CSN_PORT, L01_CSN); //Deselect chip

	return(status);
}

//Sends one byte to nRF
uint8_t send_byte(uint8_t cmd)
{
	uint8_t status;
	
	cbi(L01_CSN_PORT, L01_CSN); //Select chip
	status = tx_spi_byte(cmd);
	sbi(L01_CSN_PORT, L01_CSN); //Deselect chip
	
	return(status);
}

//Basic SPI to nRF24L01
uint8_t tx_spi_byte(uint8_t outgoing)
{
    uint8_t i, incoming;
	incoming = 0;
	

    //Send outgoing byte
    for(i = 0 ; i < 8 ; i++)
    {
		
		if(outgoing & 0b10000000)
			sbi(L01_PORT, MOSI);
		else
			cbi(L01_PORT, MOSI);

		sbi(L01_PORT, L01_SCK); //L01_SCK = 1;
		delay_busy_us(RF_DELAY);
		
		//MISO bit is valid after clock goes going high
		incoming <<= 1;
		if(L01_PORT_PIN & (1<<MISO)) incoming |= 0x01; //this line is fucking up
		
		cbi(L01_PORT, L01_SCK); //L01_SCK = 0; 
		delay_busy_us(RF_DELAY);
		
        outgoing <<= 1;
		
    }
	

	return(incoming);
}

//Configures nRF24L01 for recieve mode
void config_rx_nRF24L01(void)
{
	cbi(L01_CE_PORT, L01_CE);//CE = 0
    
	send_command(0x20, 0x3D);//PRX, 16 bit CRC enabled===========================================================
	
    send_command(0x21, 0);//dissable auto-ack for all channels====================================================
	
    send_command(0x26, 0x07);//data rate = 1MB ===============================================================
    
    send_command(0x31, sizeof(packet));//4 byte payload ==============================================================
	
    send_command(0x20, 0x3B);//PWR_UP = 1 ================================================================
    
	sbi(L01_CE_PORT, L01_CE);//CE = 1

}

//Gets data from 24L01 and puts it in rx, resets all ints
void rx_data_nRF24L01(void)
{
    uint8_t i, j, data, cmd;

    cmd = 0x61; //Read RX payload ==========================================================================================
    
	cbi(L01_CSN_PORT, L01_CSN);//CSN = 0
	delay_busy_us(RF_DELAY);
    
    for(i = 0 ; i < 8 ; i++)
    {
		if(cmd & 0b10000000)
			sbi(L01_PORT, MOSI);
		else
			cbi(L01_PORT, MOSI);
		
		sbi(L01_PORT, L01_SCK); //L01_SCK = 1;
		delay_busy_us(RF_DELAY);

		cbi(L01_PORT, L01_SCK); //L01_SCK = 0; 
		delay_busy_us(RF_DELAY);
		
        cmd <<= 1;
		
    }
    
    for (j = 0; j < sizeof(packet); j++)
    {
        data = 0;
        
        for(i = 0 ; i < 8 ; i++)
        {
            data <<= 1;
            if(L01_PORT_PIN & (1<<MISO) ) data |= 0x01;
			else data &= 0xFE;

			sbi(L01_PORT, L01_SCK); //L01_SCK = 1;
			delay_busy_us(RF_DELAY);
            
			cbi(L01_PORT, L01_SCK); //L01_SCK = 0; 
			delay_busy_us(RF_DELAY);
        }
		
		((unsigned char*)&rx)[j] = data;
 
    }
    
    sbi(L01_CSN_PORT, L01_CSN);//CSN = 1
    
    send_byte(0xE2);//Flush RX FIFO =============================================================

	send_command(0x27, 0x40);//reset int =========================================================

}

