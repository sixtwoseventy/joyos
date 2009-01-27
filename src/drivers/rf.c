#include <joyos.h>
#include <rf.h>
#include <nrf24l01.h>
#include <hal/delay.h>
#include <hal/uart.h>

void rf_rx() {
    // reset CE
    RF_CE(0);
    delay_busy_us(150);
    // power on and enable 2-byte CRC, RX mode
    nrf_write_reg(NRF_REG_CONFIG, 
            _BV(NRF_BIT_PWR_UP) | 
            _BV(NRF_BIT_EN_CRC) | 
            _BV(NRF_BIT_CRCO) | 
            _BV(NRF_BIT_PRIM_RX));
    // set CE
    RF_CE(1);
    // wait >= 130 us
    delay_busy_us(150);
}

void rf_tx() {
    // reset CE
    RF_CE(0);
    delay_busy_us(150);
    // power on and enable 2-byte CRC, TX mode
    nrf_write_reg(NRF_REG_CONFIG, 
            _BV(NRF_BIT_PWR_UP) | 
            _BV(NRF_BIT_EN_CRC) | 
            _BV(NRF_BIT_CRCO));
}

extern uint8_t team_number[2];
uint8_t pipes, addrs[6];

void rf_test() {
    rf_tx();
    nrf_write_reg(NRF_REG_RF_SETUP, 
            (NRF_RF_PWR_0DB << NRF_RF_PWR_BASE) | 
            (NRF_RF_DR_250KBPS << NRF_BIT_RF_DR_BASE) | 0x90);
    delay_busy_us(150);
    RF_CE(1);

    delay_busy_ms(100);

    RF_CE(0);
    delay_busy_us(150);
    nrf_write_reg(NRF_REG_RF_SETUP, 
            (NRF_RF_PWR_0DB << NRF_RF_PWR_BASE) | 
            (NRF_RF_DR_250KBPS << NRF_BIT_RF_DR_BASE));
    rf_rx();
}

void rf_init() {
    uint8_t i;
    // count the 'tubes
    //uart_printf("teams: %02X%02X\n", team_number[0], team_number[1]);
    while(team_number[pipes++]);pipes--;
    for(i=0;i<pipes;i++)
        addrs[i] = team_number[i];
    //uart_printf("pipes = %d\n", pipes);

    delay_busy_ms(100);
    // nRF24L01+ is in Power Down

    // turn the thing on
    nrf_write_reg(NRF_REG_CONFIG, 
            _BV(NRF_BIT_PWR_UP) | 
            _BV(NRF_BIT_PRIM_RX));
    // wait Tpd2stby to start up crystal
    delay_busy_ms(5);

    // nRF24L01+ is now in Standby I

    // enable auto acknowledge on the right number of pipes
    nrf_write_reg(NRF_REG_EN_AA, (uint8_t)(1<<pipes)-1);
    // set to use 3-byte addresses
    nrf_write_reg(NRF_REG_SETUP_AW, NRF_AW_3); 
    // 250us auto retransmit delay, 15 retries
    nrf_write_reg(NRF_REG_SETUP_RETR, 0x1F);
    // RF channel 2 (doesn't matter as long as it's constant)
    nrf_write_reg(NRF_REG_RF_CH, 0x02);
    // 0 dBm TX power, 250kbps data rate
    nrf_write_reg(NRF_REG_RF_SETUP, 
            (NRF_RF_PWR_0DB << NRF_RF_PWR_BASE) | 
            (NRF_RF_DR_250KBPS << NRF_BIT_RF_DR_BASE));
    // enable dynamic payload lengths
    nrf_write_reg(NRF_REG_FEATURE, _BV(NRF_BIT_EN_DPL));
    nrf_write_reg(NRF_REG_DYNPD, (uint8_t)(1<<pipes)-1);

    // pipes 0 and 1 have full 3-byte addresses.  pipes 2-5 share the MSBs of pipe 1.
    // all addresses will be in the format 0x6270XX,
    // where XX is either a team number or 0x80 | team number
    uint8_t addr[3];
    // LSB first
    addr[0] = addrs[0];
    // magic 0x6270 prefix
    addr[1] = 0x70;
    addr[2] = 0x62;
    nrf_write_multibyte_reg(NRF_REG_RX_ADDR_P0, addr, 3);
    if (pipes>1) {
        addr[0] = addrs[1];
        nrf_write_multibyte_reg(NRF_REG_RX_ADDR_P0+1, addr, 3);
    }
    for(i=2;i<pipes;i++)
        nrf_write_reg(NRF_REG_RX_ADDR_P0+i, addrs[i]);
    addr[0] = 0x00;
    nrf_write_multibyte_reg(NRF_REG_TX_ADDR, addr, 3);

    // enable the right number of pipes
    nrf_write_reg(NRF_REG_EN_RXADDR, (uint8_t)(1<<pipes)-1);
    // start receiving
    rf_rx();
}

uint8_t rf_send_packet(uint8_t address, uint8_t *data, uint8_t len) {
    rf_tx();
    // preserve pipe 0 address
    uint8_t pipe0_addr = nrf_read_reg(NRF_REG_RX_ADDR_P0);
    // listen for ACK
    nrf_write_reg(NRF_REG_RX_ADDR_P0, address);
    // write TX address LSB first
    nrf_write_reg(NRF_REG_TX_ADDR, address);
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
    uart_printf_P(PSTR("TX ACK status: %02X\n"), status);
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

uint8_t rf_send(uint8_t address, uint8_t *data, uint8_t len) {
    // send in 32-byte chunks
    uint8_t i, n;
    for(i=0;i<len;i+=32) {
        n = (len-i)>32?32:(len-i);
        rf_send_packet(address, data+i, n);
    }
    return 1;
}

uint8_t rf_recv_pipe(uint8_t *data, uint8_t *len, uint8_t *pipe, uint8_t block) {
    rf_rx();
    // read STATUS
    uint8_t status;
    do {
        status = nrf_read_reg(NRF_REG_STATUS);
        *pipe = (status >> NRF_RX_P_NO_BASE) & 0x07;
        if ((status & _BV(NRF_BIT_RX_DR)) != 0 || *pipe != NRF_RX_P_NO_EMPTY)
            break;
        yield();
    } while (block);
    uart_printf_P(PSTR("RX pipe: %02X\n"), *pipe);
    // return if no packet
    if (*pipe >= 6)
        return 0;
    // read rx payload length
    *len = nrf_read_reg(NRF_REG_RX_PW_P0 + *pipe);
    uart_printf_P(PSTR("RX len: %02X\n"), *len);
    if (*len > 32) {
        // clear error condition
        nrf_flush_rx();
        return 0;
    }
    if (*len == 0)
        return 0;
    // read rx payload
    nrf_read_rx_payload(data, *len);
    // clear rx_dr irq
    nrf_write_reg(NRF_REG_STATUS, _BV(NRF_BIT_RX_DR));
    return 1;
}

uint8_t rf_recv(uint8_t *data, uint8_t *len) {
    // "default" receive behavior
    uint8_t pipe;
    return rf_recv_pipe(data, len, &pipe, 0);
}
