// Include headers from OS
#include <config.h>
#include <hal/delay.h>
#include <rf.h>
#include <string.h> //memcpy
#include <nrf24l01.h>
#include <kern/global.h>
#include <kern/thread.h>
#include <kern/lock.h>
#include <avr/interrupt.h>
#include <hal/uart.h>
#include <hal/io.h>

packet_buffer tx, rx;

volatile uint32_t position_microtime;
volatile board_coord objects[4];

board_coord goal_position; //The target position received from a goal packet

FILE rfio = FDEV_SETUP_STREAM(rf_put, NULL, _FDEV_SETUP_WRITE);

volatile char rf_str_buf[PAYLOAD_SIZE+1];
uint8_t rf_ch_count = 0;

volatile uint8_t rf_new_str;

struct lock rf_lock;

int rf_put(char ch, FILE *f){
    ATOMIC_BEGIN;

    tx.payload.array[rf_ch_count++] = ch;
    
    if ((ch=='\n') || (rf_ch_count == PAYLOAD_SIZE)){
        tx.type = STRING;
        rf_send_packet(0xE7, (uint8_t*)(&tx), sizeof(packet));
        rf_ch_count = 0;
    }

    ATOMIC_END;

    return ch;
}

int rf_vprintf(const char *fmt, va_list ap) {
	int count;
	acquire(&rf_lock);
	count = vfprintf(&rfio, fmt, ap);
	release(&rf_lock);

	return count;
}

int rf_printf(const char *fmt, ...) {
	va_list ap;
	int count;

	va_start(ap, fmt);
	count = rf_vprintf(fmt, ap);
	va_end(ap);

	return count;
}

int rf_vprintf_P(const char *fmt, va_list ap) {
	int count;
	acquire(&rf_lock);
	count = vfprintf_P(&rfio, fmt, ap);
	release(&rf_lock);

	return count;
}

int rf_printf_P(const char *fmt, ...) {
	va_list ap;
	int count;

	va_start(ap, fmt);
	count = rf_vprintf_P(fmt, ap);
	va_end(ap);

	return count;
}

void rf_rx(void)
{
	RF_CE(0);
    delay_busy_us(150);
/*
    // RESOLVE THIS WITH THAT OTHER THING BELOW
    // power on and enable 2-byte CRC, RX mode
    nrf_write_reg(NRF_REG_CONFIG, 
            _BV(NRF_BIT_EN_CRC) | 
            _BV(NRF_BIT_CRCO) | 
            _BV(NRF_BIT_PRIM_RX));
*/
	nrf_write_reg(NRF_REG_CONFIG, 0x3D); // PRX, 16 bit CRC enabled
    nrf_write_reg(NRF_REG_EN_AA, 0); // disable auto-ack for all channels
    nrf_write_reg(NRF_REG_RF_SETUP, 0x07); // data rate = 1MB
    nrf_write_reg(NRF_REG_RX_PW_P0, sizeof(packet));
    nrf_write_reg(NRF_REG_CONFIG, 0x3B); // PWR_UP = 1
	RF_CE(1);
    // wait >= 130 us
    delay_busy_us(150);
}

uint8_t
rf_tx(void) {
    RF_CE(0);
    delay_busy_us(150);
/*
    // RESOLVE THIS WITH THAT OTHER THING BELOW
    nrf_write_reg(NRF_REG_CONFIG, 
            _BV(NRF_BIT_EN_CRC) | 
            _BV(NRF_BIT_CRCO));
*/

    nrf_write_reg(NRF_REG_CONFIG, 0x7C); //16 bit CRC enabled, be a transmitter
    nrf_write_reg(NRF_REG_EN_AA, 0x00); //Disable auto acknowledge on all pipes
    nrf_write_reg(NRF_REG_SETUP_RETR, 0x00); //Disable auto-retransmit
    nrf_write_reg(NRF_REG_SETUP_AW, 0x03); //Set address width to 5bytes (default, not really needed)
    nrf_write_reg(NRF_REG_RF_SETUP, 0x07); //Air data rate 1Mbit, 0dBm, Setup LNA
    nrf_write_reg(NRF_REG_RF_CH, 0x02); //RF Channel 2 (default, not really needed)
    uint8_t addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    nrf_write_multibyte_reg(NRF_REG_TX_ADDR, addr, 5);
    nrf_write_reg(NRF_REG_CONFIG, 0x7A); //Power up, be a transmitter
    return nrf_read_status();
}

uint8_t rf_send_packet(uint8_t address, uint8_t *data, uint8_t len) {
	rf_tx();
    // preserve pipe 0 address
    uint8_t pipe0_addr = nrf_read_reg(NRF_REG_RX_ADDR_P0);
    // listen for ACK to this address
    nrf_write_reg(NRF_REG_RX_ADDR_P0, address);
    // write TX address LSB first
    nrf_write_reg(NRF_REG_TX_ADDR, address);

    nrf_write_reg(NRF_REG_STATUS, 0x7E); //Clear any interrupts

    nrf_write_tx_payload(data, len);
    // start transmission
    RF_CE(1);
    delay_busy_us(20);
    RF_CE(0);
    // wait for ACK
    uint8_t status;
    while(((status = nrf_read_reg(NRF_REG_STATUS)) & (_BV(NRF_BIT_TX_DS) | _BV(NRF_BIT_MAX_RT))) == 0) {
        yield();
    }
    //printf("TX ACK status: %02X\n", status);
    // clear transmit interrupt conditions
    nrf_write_reg(NRF_REG_STATUS, status & (_BV(NRF_BIT_TX_DS) | _BV(NRF_BIT_MAX_RT)));
    // restore pipe 0 address
    nrf_write_reg(NRF_REG_RX_ADDR_P0, pipe0_addr);
    // flush TX FIFO
    nrf_flush_tx();
    // return to RX mode
    rf_rx();
    return (status & _BV(NRF_BIT_TX_DS)) != 0;
}

void rf_process_packet (packet_buffer *rx, uint8_t pipe) {

    uint8_t type = rx->type;
    //uint8_t address = rx->address;

    switch (type) {
        case POSITION:

            memcpy(objects, rx->payload.coords, sizeof(objects));
            position_microtime = get_time_us();

            break;
        case GOAL:

            goal_position = rx->payload.coords[0];

            break;

/*		case START:
    
            //Remaining bytes are robots which are starting.  Check if we're one of them.
            for (uint8_t i = 0; i < 30; i++) {
                target = rx->packet_buf.payload[i];
                if (target == self_id) { round_start(); break; }
            }
            break;
        case STOP:
            //Remaining bytes are robots which are stopping.  Check if we're one of them.
            for (uint8_t i = 0; i < 30; i++) {
                target = rx->packet_buf.payload[i];
                if (target == self_id) { round_end(); break; }
            }
            break;*/

        case STRING:
            rf_new_str = 1;
            memcpy(rf_str_buf, rx->payload.array, PAYLOAD_SIZE);
            break;
        default:
            break;
    }
}

// get a packet; return pipe number
uint8_t
rf_get_packet(uint8_t *buf, uint8_t *size) {
    uint8_t pipe;
    while (1) {
        pipe = ((nrf_read_reg(NRF_REG_STATUS) & NRF_RX_P_NO_MASK) >> NRF_RX_P_NO_BASE);
        if (pipe == NRF_RX_P_NO_EMPTY)
            return pipe;
        *size = nrf_read_rx_payload_len();
        if (*size > 32) {
            nrf_flush_rx();
            continue;
        }
        break;
    }
    nrf_read_rx_payload(buf, *size);
    return pipe;
}

int rf_receive (void) {
    for (;;) {
        //if (PINE & _BV(PD7)) {
            uint8_t status = nrf_read_status();
            if (status & _BV(NRF_BIT_RX_DR)) {
                nrf_write_reg(NRF_REG_STATUS, _BV(NRF_BIT_RX_DR)); //reset int

                packet_buffer rx;
                uint8_t size;
                uint8_t pipe;
                while ((pipe = rf_get_packet((uint8_t*)&rx, &size)) != NRF_RX_P_NO_EMPTY)
                    rf_process_packet(&rx, pipe);
            }
        //}
        yield();
    }
    return 0;
}

// Initialize RF
void rf_init (void) {
    // The rf_lock ensures that printing to
    // rf from several threads will not cause
    // characters to be interleaved between threads
    init_lock(&rf_lock, "RF Lock");

    // STRING packets don't contain the null character
    // for efficiency.  rf_str_buf is one character larger
    // than the payload to hold this additional character
    rf_str_buf[PAYLOAD_SIZE] = '\0';

    rf_new_str = 0;

	rf_rx(); //Enable receive mode

	create_thread (&rf_receive, STACK_DEFAULT, 0, "rf");
}

