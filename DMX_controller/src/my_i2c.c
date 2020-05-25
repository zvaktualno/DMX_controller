#include <asf.h>
#include <string.h>
#include "config.h"
#include "my_i2c.h"

volatile bool i2c_read_is_complete = 0;
volatile bool i2c_write_is_complete = 0;
extern volatile uint8_t TWI_counter;
uint8_t i2c_tx_buffer[TWI_BUFFER_SIZE];
uint8_t i2c_rx_buffer[TWI_BUFFER_SIZE];

void i2c_write_complete_callback(struct i2c_master_module *const module) {
    /* Initiate new packet read */
    i2c_write_is_complete = 1;
}

void i2c_read_complete_callback(struct i2c_master_module *const module) {
    i2c_read_is_complete = 1;
}

void configure_i2c_callbacks(void) {
    /* Register callback function. */

    i2c_master_register_callback(&i2c_master_instance, i2c_write_complete_callback, I2C_MASTER_CALLBACK_WRITE_COMPLETE);
    i2c_master_enable_callback(&i2c_master_instance, I2C_MASTER_CALLBACK_WRITE_COMPLETE);

    i2c_master_register_callback(&i2c_master_instance, i2c_read_complete_callback, I2C_MASTER_CALLBACK_READ_COMPLETE);
    i2c_master_enable_callback(&i2c_master_instance, I2C_MASTER_CALLBACK_READ_COMPLETE);
}

void configure_i2c(void) {
    /* Initialize config structure and software module */
    struct i2c_master_config config_i2c_master;
    i2c_master_get_config_defaults(&config_i2c_master);

    /* Change buffer timeout to something longer */
    config_i2c_master.buffer_timeout	= 10000;
    config_i2c_master.pinmux_pad0		= TWI_SDA_PINMUX;
    config_i2c_master.pinmux_pad1		= TWI_SCL_PINMUX;
    config_i2c_master.generator_source	= GCLK_GENERATOR_2;
    config_i2c_master.start_hold_time	= I2C_MASTER_START_HOLD_TIME_300NS_600NS;
    config_i2c_master.baud_rate			= I2C_MASTER_BAUD_RATE_100KHZ;
    /* Initialize and enable device with config */
    while(i2c_master_init(&i2c_master_instance, TWI_MODULE, &config_i2c_master) != STATUS_OK);

    i2c_master_enable(&i2c_master_instance);
}
void TWI_write(uint8_t address, uint8_t data_length, uint8_t *p_data) {
    wr_packet.address = address;
    wr_packet.data_length = data_length;
    wr_packet.data = p_data;
    i2c_write_is_complete = 0;
    TWI_counter = 5;
    while(i2c_master_write_packet_job(&i2c_master_instance, &wr_packet) && TWI_counter);
    while(!i2c_write_is_complete && TWI_counter);
    if(!TWI_counter)
        i2c_master_cancel_job(&i2c_master_instance);
}

enum status_code TWI_read(uint16_t send_address, uint8_t *send_data, uint16_t recieveBytes) {
    TWI_counter = 5;//set timeout

    wr_packet.address = send_address;
    wr_packet.data = send_data;
    wr_packet.data_length = 1;

    rd_packet.address = send_address;
    rd_packet.data_length = recieveBytes;
    rd_packet.data = i2c_rx_buffer;

    /* Clear flags */
    i2c_read_is_complete = 0;
    i2c_write_is_complete = 0;

    while(i2c_master_write_packet_job(&i2c_master_instance, &wr_packet) != STATUS_OK && TWI_counter);
    while(!i2c_write_is_complete && TWI_counter);
    if(!TWI_counter) {
        i2c_master_cancel_job(&i2c_master_instance);
        return STATUS_ERR_TIMEOUT;
    }
    while(i2c_master_read_packet_job(&i2c_master_instance, &rd_packet) != STATUS_OK && TWI_counter);

    while(!i2c_read_is_complete && TWI_counter);
    if(!TWI_counter) {
        i2c_master_cancel_job(&i2c_master_instance);
        return STATUS_ERR_TIMEOUT;
    }
    return STATUS_OK;
}

unsigned char *Get_TWI_Buffer(void) {
    //return pointer to i2c data
    return i2c_tx_buffer;
}

unsigned char *Get_TWI_rx_Buffer(void) {
    //return pointer to i2c data
    return i2c_rx_buffer;

}