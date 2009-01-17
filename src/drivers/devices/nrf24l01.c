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

#include "config.h"
#include "hal/io.h"
#include "hal/spi.h"
#include "nrf24l01.h"

uint8_t
nrf_read_reg(uint8_t reg) {
    spi_acquire ();
    uint8_t cmd[2];
    cmd[0] = NRF_SPI_R_REGISTER | reg;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,2);
    SPI_RF_SS(1);
    spi_release ();
    return cmd[1];
}

void
nrf_read_multibyte_reg(uint8_t reg, uint8_t *data, uint8_t len) {
    spi_acquire ();
    uint8_t cmd[6];
    cmd[0] = NRF_SPI_R_REGISTER | reg;
    uint8_t i;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,len+1);
    SPI_RF_SS(1);
    spi_release ();
    for(i=0;i<len;i++)
        data[i] = cmd[i+1];
}

void
nrf_write_reg(uint8_t reg, uint8_t data) {
    spi_acquire ();
    uint8_t cmd[2];
    cmd[0] = NRF_SPI_W_REGISTER | reg;
    cmd[1] = data;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,2);
    SPI_RF_SS(1);
    spi_release ();
}

void
nrf_write_multibyte_reg(uint8_t reg, uint8_t *data, uint8_t len) {
    spi_acquire ();
    uint8_t cmd[6];
    cmd[0] = NRF_SPI_W_REGISTER | reg;
    uint8_t i;
    for(i=0;i<len;i++)
        cmd[i+1] = data[i];
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,len+1);
    SPI_RF_SS(1);
    spi_release ();
}

uint8_t nrf_read_rx_payload_len() {
    spi_acquire ();
    uint8_t cmd[2];
    cmd[0] = NRF_SPI_R_RX_LP_WID;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,2);
    SPI_RF_SS(1);
    spi_release ();
    return cmd[1];
}

void
nrf_read_rx_payload(uint8_t *data, uint8_t len) {
    spi_acquire ();
    uint8_t cmd[9];
    cmd[0] = NRF_SPI_R_RX_PAYLOAD;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,len+1);
    SPI_RF_SS(1);
    spi_release ();
    uint8_t i;
    for (i=0;i<len;i++)
        data[i] = cmd[i+1];
}

void
nrf_flush_tx() {
    spi_acquire ();
    uint8_t cmd[1];
    cmd[0] = NRF_SPI_FLUSH_TX;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,1);
    SPI_RF_SS(1);
    spi_release ();
}

void
nrf_flush_rx() {
    spi_acquire ();
    uint8_t cmd[1];
    cmd[0] = NRF_SPI_FLUSH_RX;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,1);
    SPI_RF_SS(1);
    spi_release ();
}

void
nrf_reuse_tx_pl() {
    spi_acquire ();
    uint8_t cmd[1];
    cmd[0] = NRF_SPI_REUSE_TX_PL;
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,1);
    SPI_RF_SS(1);
    spi_release ();
}

void
nrf_write_tx_payload(uint8_t *data, uint8_t len) {
    spi_acquire ();
    uint8_t cmd[9];
    cmd[0] = NRF_SPI_W_TX_PAYLOAD;
    uint8_t i;
    for(i=0;i<len;i++)
        cmd[i+1] = data[i];
    spi_set_master(SPI_CLK_DIV_16, SPI_FLAGS_DEFAULT);
    SPI_RF_SS(0);
    spi_transfer_sync(cmd,len+1);
    SPI_RF_SS(1);
    spi_release ();
}

void
nrf_init_rx() {
    spi_acquire ();
    nrf_write_reg(NRF_REG_EN_AA,        0x00); // set pipe0 for auto-ack
    nrf_write_reg(NRF_REG_EN_RXADDR,    0x01); // set pipe0 for reception
    nrf_write_reg(NRF_REG_RX_PW_P0,     0x07); // set pipe0 for 1 byte payload
    nrf_write_reg(NRF_REG_CONFIG,       0x0B); // EN_CRC|PWR_UP|PRIM_RX
    spi_release ();
}
