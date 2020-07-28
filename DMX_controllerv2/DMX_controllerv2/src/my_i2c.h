#ifndef REC_I2C_H_
#define REC_I2C_H_

void configure_i2c_callbacks(void);

void configure_i2c(void);
void TWI_write(uint8_t address, uint8_t data_length, uint8_t *p_data);

enum status_code TWI_read(uint16_t send_address, uint8_t *send_data, uint16_t recieveBytes);
uint8_t *I2C_get_tx_Buffer(void);
uint8_t *I2C_get_rx_Buffer(void);
#endif /* REC_I2C_H_ */