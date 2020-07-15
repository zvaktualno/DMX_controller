#ifndef REC_I2C_H_
#define REC_I2C_H_

struct i2c_master_packet rd_packet, wr_packet;
struct i2c_master_module i2c_master_instance;
enum status_code TWI_read(uint16_t send_address, uint8_t *send_data, uint16_t recieveBytes);
void TWI_write(uint8_t address, uint8_t data_length, uint8_t *p_data);

unsigned char *I2C_get_tx_Buffer(void);
unsigned char *I2C_get_rx_Buffer(void);

void configure_i2c(void);
void configure_i2c_callbacks(void);
void i2c_write_complete_callback(struct i2c_master_module *const module);

void set_RTC_time(unsigned char ure, unsigned char minute, unsigned char sekunde );
void set_RTC_date(unsigned char dan, unsigned char mesec, unsigned char leto );
void read_RTC_date(void);
void read_RTC_time(void);
uint32_t RTC_get_bcd_date(void);
uint8_t RTC_read_bcd_hours(void);

void read_error_data(unsigned char naslov_RTC_error_branje);
void set_error_data(unsigned char st_napake, uint8_t st_elementa, unsigned char ura, unsigned char minuta, unsigned char sekunda, unsigned char dan, unsigned char mesec, unsigned char leto, unsigned char naslov_RTC_error_pisanje );

void write_RTC_SRAM(uint8_t mem_loc, uint8_t data);
uint8_t read_RTC_SRAM(uint8_t mem_loc);



#endif /* REC_I2C_H_ */