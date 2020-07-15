#include <asf.h>
#include <stdint.h>
#include "i2c_fs.h"
#include "my_i2c.h"
#include "config.h"

void read_memory_map(I2C_FS *fs) {
    uint8_t i2c_packet[] = {0};
    if(TWI_read(EEPROM_ADDR, i2c_packet, 8) != STATUS_OK);
    fs->memory_map = *(uint64_t *)I2C_get_rx_Buffer();

}

void write_memory_map(I2C_FS *fs) {
    uint8_t i2c_packet[9] = {0};
    for(uint8_t i = 0; i < 8; i++)
        i2c_packet[i + 1] = (fs->memory_map >> (i * 8)) & 0xFF;
    TWI_write(EEPROM_ADDR, 9, i2c_packet);
}

static uint16_t get_position(uint32_t mem_map) {
    for(uint8_t i = 0; i < 32; i++)
        if(mem_map & (1UL << i))return i * sizeof(PRESET) * +64;
}

static void get_num_of_saves(I2C_FS *fs) {
    uint8_t stevec = 0;
    for(uint8_t i = 0; i < MAX_SAVES; i++) {
        if(fs->memory_map & (1UL << i))
            stevec++;
    }
    fs->num_of_saves = stevec;
}

void read_memory(PRESET *preset) {
    preset->position = get_position(0);
    uint8_t rec_buffer[sizeof(PRESET)];

    if(TWI_read(EEPROM_ADDR, &preset->position, sizeof(PRESET)) != STATUS_OK)
        return;
    uint8_t *p_to_preset = (uint8_t *)preset;
    for(uint16_t i = 0; i < sizeof(PRESET); i++)
        *(p_to_preset++) = rec_buffer;
}

void write_memory(PRESET *preset) {
    int16_t remaining_size = sizeof(PRESET);
    uint8_t *p_to_preset = (uint8_t *)preset;
    uint8_t rec_buffer[66];
    //64byte page write + 2bits for address;

    while(remaining_size > 0) {
        rec_buffer[0] = (preset->position + sizeof(PRESET) - remaining_size) >> 8;
        rec_buffer[1] = (preset->position + sizeof(PRESET) - remaining_size) & 0xFF;

        for(uint16_t i = 2; i < 66; i++)
            *(rec_buffer + i) = *(p_to_preset++);
        remaining_size -= 64;
        TWI_write(EEPROM_ADDR, rec_buffer, 66);
        return;
        delay_ms(5);
    }

}