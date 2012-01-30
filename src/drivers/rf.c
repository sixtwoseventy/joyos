#ifndef SIMULATE

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
#include <rf.h>
#include <hal/spi.h>
#include <hal/io.h>
#include <motor.h>
#include <kern/thread.h>

#else

#include <joyos.h>
#include <stdarg.h>
#include <socket.h>
#include <stdio.h>

#endif

struct lock objects_lock;
volatile game_data game;
volatile game_data locked_game;
volatile uint32_t position_microtime;
volatile uint32_t locked_position_microtime;

#ifndef SIMULATE

volatile uint8_t robot_id = 0xFF;

volatile int rf_flush_count = 0;

uint8_t rf_get_packet(uint8_t *buf, uint8_t *size);
void rf_process_packet (packet_buffer *rx, uint8_t size, uint8_t pipe);
uint8_t rf_send_packet(uint8_t address, uint8_t *data, uint8_t len);

void rf_poll(pipe *p) {
    if (try_acquire(&p->tx_buf.lock)) {
        // send a packet if we have MIN(29, rf_flush_count) bytes
        int n = (rf_flush_count == 0 || rf_flush_count > PAYLOAD_SIZE) ? PAYLOAD_SIZE : rf_flush_count;
        if (ring_size(&p->tx_buf) >= n) {
            packet_buffer tx;
            tx.type = STRING;
            tx.seq_no = robot_id;
            ring_read(&p->tx_buf, tx.payload.array, n);
            rf_send_packet(0xE7, (uint8_t*)(&tx), 2 + n);
            rf_flush_count -= n;
            if (rf_flush_count < 0)
                rf_flush_count = 0;
        }
        release(&p->tx_buf.lock);
    }
    if (try_acquire(&p->rx_buf.lock)) {
        //if (PINE & _BV(PD7)) {
        uint8_t status = nrf_read_status();
        if (status & _BV(NRF_BIT_RX_DR)) {
            nrf_write_reg(NRF_REG_STATUS, _BV(NRF_BIT_RX_DR)); //reset int

            packet_buffer rx;
            uint8_t size;
            uint8_t pipe;
            while ((pipe = rf_get_packet((uint8_t*)&rx, &size)) != NRF_RX_P_NO_EMPTY)
                rf_process_packet(&rx, size, pipe);
        }
        //}
        release(&p->rx_buf.lock);
    }
}

int rf_fflush(pipe *p) {
    acquire(&p->tx_buf.lock);
    ATOMIC_BEGIN;
    rf_flush_count = ring_size(&p->tx_buf);
    ATOMIC_END;
    release(&p->tx_buf.lock);
    return 0;
}

void rf_rx(void) {
    RF_CE(0);
    delay_busy_us(150); // I don't think delay_busy_us actually goes up that high
    nrf_write_reg(NRF_REG_CONFIG,
            _BV(NRF_BIT_PRIM_RX) |
            _BV(NRF_BIT_CRCO) |
            _BV(NRF_BIT_EN_CRC) |
            _BV(NRF_BIT_MASK_MAX_RT) |
            _BV(NRF_BIT_MASK_TX_DR)); // PRX, 16 bit CRC enabled
    nrf_write_reg(NRF_REG_EN_AA, 0); // disable auto-ack for all channels
    nrf_write_reg(NRF_REG_RF_SETUP,
            (NRF_RF_PWR_0DB << NRF_RF_PWR_BASE) |
            (NRF_RF_DR_1MBPS << NRF_BIT_RF_DR_BASE) |
            _BV(NRF_BIT_LNA_HCURR)); // data rate = 1MB
    nrf_write_reg(NRF_REG_RX_PW_P0, sizeof(packet_buffer));
    nrf_write_reg(NRF_REG_CONFIG,
            _BV(NRF_BIT_PRIM_RX) |
            _BV(NRF_BIT_PWR_UP) |
            //_BV(NRF_BIT_CRCO) | // this bit was not in the configuration for some unknown reason
            _BV(NRF_BIT_EN_CRC) |
            _BV(NRF_BIT_MASK_MAX_RT) |
            _BV(NRF_BIT_MASK_TX_DR)); // PWR_UP = 1
    RF_CE(1);
    // wait >= 130 us
    delay_busy_us(150);
}

uint8_t rf_tx(void) {
    RF_CE(0);
    delay_busy_us(150);
    nrf_write_reg(NRF_REG_CONFIG,
            _BV(NRF_BIT_CRCO) |
            _BV(NRF_BIT_EN_CRC) |
            _BV(NRF_BIT_MASK_MAX_RT) |
            _BV(NRF_BIT_MASK_TX_DR) |
            _BV(NRF_BIT_MASK_RX_DR)); //16 bit CRC enabled, be a transmitter
    nrf_write_reg(NRF_REG_EN_AA, 0); //Disable auto acknowledge on all pipes
    nrf_write_reg(NRF_REG_SETUP_RETR, 0); //Disable auto-retransmit
    nrf_write_reg(NRF_REG_SETUP_AW, NRF_AW_5); //Set address width to 5bytes (default, not really needed)
    nrf_write_reg(NRF_REG_RF_SETUP,
            (NRF_RF_PWR_0DB << NRF_RF_PWR_BASE) |
            (NRF_RF_DR_1MBPS << NRF_BIT_RF_DR_BASE) |
            _BV(NRF_BIT_LNA_HCURR)); //Air data rate 1Mbit, 0dBm, Setup LNA
    nrf_write_reg(NRF_REG_RF_CH, 2); //RF Channel 2 (default, not really needed)
    uint8_t addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    nrf_write_multibyte_reg(NRF_REG_TX_ADDR, addr, 5);
    nrf_write_reg(NRF_REG_CONFIG,
            _BV(NRF_BIT_PWR_UP) |
            //_BV(NRF_BIT_CRCO) |
            _BV(NRF_BIT_EN_CRC) |
            _BV(NRF_BIT_MASK_MAX_RT) |
            _BV(NRF_BIT_MASK_TX_DR) |
            _BV(NRF_BIT_MASK_RX_DR)); //Power up, be a transmitter
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

uint8_t rf_which_board = 0xFF;
void rf_process_packet (packet_buffer *rx, uint8_t size, uint8_t pipe) {
    uint8_t type = rx->type;

    switch (type) {
        case POSITION:
            if (robot_id != 0xFF) {
                // if we're in position 1, swap to position 0
                if (rx->payload.game.coords[1].id == robot_id) {
                    board_coord t = rx->payload.game.coords[0];
                    rx->payload.game.coords[0] = rx->payload.game.coords[1];
                    rx->payload.game.coords[1] = t;
                }
                // if we don't know which board we're on, look for us in position 0
                if (rf_which_board == 0xFF && rx->payload.game.coords[0].id == robot_id)
                    rf_which_board = rx->board;
                // if this packet is for the board we're on, save it
                if (rf_which_board == rx->board) {
                    acquire(&objects_lock);

                    // if this packet doesn't contain our location, we'll just keep our previous coords
                    if (rx->payload.game.coords[0].id != robot_id) {

                        // if our opponent is in coords[0], copy them to coords[1] instead
                        if (rx->payload.game.coords[0].id != 0xAA) {
                            rx->payload.game.coords[1] = rx->payload.game.coords[0];
                        }

                        // restore our previous coords
                        rx->payload.game.coords[0] = locked_game.coords[0];
                    }

                    memcpy((char *)&locked_game, &rx->payload.game, sizeof(rx->payload.game));
                    uint32_t time_us = get_time_us();
                    locked_position_microtime = time_us;
                    release(&objects_lock);
                }
            }
            break;

        case START:
            if (robot_id != 0xFF) {
                //Remaining bytes are robots which are starting.  Check if we're one of them.
                for (uint8_t i = 0; i < 30; i++) {
                    uint8_t target = rx->payload.array[i];
                    if (target == robot_id) { round_start(); break; }
                }
            }
            break;

        case STOP:
            if (robot_id != 0xFF) {
                //Remaining bytes are robots which are stopping.  Check if we're one of them.
                for (uint8_t i = 0; i < 30; i++) {
                    uint8_t target = rx->payload.array[i];
                    if (target == robot_id) { round_end(); break; }
                }
            }
            break;

        case REPLY_STRING:
            if (rx->seq_no == BROADCAST || rx->seq_no == robot_id)
                ring_write(&std_pipes[PIPE_RF].rx_buf, rx->payload.array, size - 2);
            break;

        default:
            break;
    }
}

//copy the locked_objects[] into the user-accessible objects[] array - this way users cannot stall the rf thread by holding the objects_lock too long
void copy_objects(){
    acquire(&objects_lock);
    memcpy((char*)&game, (char*)&locked_game, sizeof(locked_game));
    position_microtime = locked_position_microtime;
    release(&objects_lock);
}

// get a packet; return pipe number
uint8_t rf_get_packet(uint8_t *buf, uint8_t *size) {
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

void rf_init (void) {
    static uint8_t first_call = 1;
    if (first_call) {
        first_call = 0;
        rf_rx(); //Enable receive mode
        init_lock(&objects_lock, "objects[] lock");
    }
}

#endif
