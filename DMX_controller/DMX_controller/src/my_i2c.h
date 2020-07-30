#ifndef REC_I2C_H_
#define REC_I2C_H_

void configure_i2c(void);
void configure_i2c_callbacks(void);

void i2c_write(uint8_t address, uint8_t *p_data, uint8_t data_length);
void i2c_read(uint16_t address, uint8_t *p_data, uint16_t recieveBytes);

uint8_t *i2c_get_tx_Buffer(void);
uint8_t *i2c_get_rx_Buffer(void);
#endif /* REC_I2C_H_ */