void rf_init();
uint8_t rf_send(uint8_t address, uint8_t *data, uint8_t len);
uint8_t rf_recv(uint8_t *data, uint8_t *len);
