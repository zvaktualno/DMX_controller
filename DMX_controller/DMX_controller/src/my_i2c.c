#include <asf.h>
#include <system_interrupt.h>
#include <string.h>
#include "my_i2c.h"
#include "config.h"

volatile uint8_t i2c_read_is_complete = 0;
volatile uint8_t i2c_write_is_complete = 0;

uint8_t i2c_tx_buffer[TWI_BUFFER_SIZE];
uint8_t i2c_rx_buffer[sizeof(PRESET) + 10];

struct i2c_master_packet rd_packet, wr_packet;
struct i2c_master_module i2c_master_instance;

void i2c_write_complete_callback(struct i2c_master_module *const module) {
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
    config_i2c_master.pinmux_pad0		= PINMUX_PA22C_SERCOM3_PAD0;
    config_i2c_master.pinmux_pad1		= PINMUX_PA23C_SERCOM3_PAD1;
    config_i2c_master.generator_source	= GCLK_GENERATOR_2;
    config_i2c_master.start_hold_time	= I2C_MASTER_START_HOLD_TIME_DISABLED;
    config_i2c_master.baud_rate			= I2C_BAUDRATE;
    /* Initialize and enable device with config */
    while(i2c_master_init(&i2c_master_instance, I2C_MODULE, &config_i2c_master) != STATUS_OK);

    i2c_master_enable(&i2c_master_instance);
    configure_i2c_callbacks();
}

void i2c_write(uint8_t address, uint8_t *p_data, uint8_t data_length) {
    wr_packet.address = address;
    wr_packet.data_length = data_length;
    wr_packet.data = p_data;
    i2c_write_is_complete = 0;


    while(i2c_master_write_packet_job(&i2c_master_instance, &wr_packet) != STATUS_OK);
    while(!i2c_write_is_complete);
    delay_ms(5); // Required for pagewrite refresh
}

void i2c_read(uint16_t address, uint8_t *p_data, uint16_t recieveBytes) {

    i2c_write(address, p_data, 2);

    rd_packet.address = address;
    rd_packet.data_length = recieveBytes;
    rd_packet.data = i2c_rx_buffer;

    i2c_read_is_complete = 0;
    while(i2c_master_read_packet_job(&i2c_master_instance, &rd_packet) != STATUS_OK);
    while(!i2c_read_is_complete);
    return;
}

uint8_t *i2c_get_tx_Buffer(void) {
    return i2c_tx_buffer;
}

uint8_t *i2c_get_rx_Buffer(void) {
    return i2c_rx_buffer;
}