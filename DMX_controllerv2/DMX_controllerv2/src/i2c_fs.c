#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "my_i2c.h"
#include "i2c_fs.h"


I2C_FS fs;
/* Gets all saves. */
void memory_init(void) {
    read_memory_map();
    get_num_of_saves();
}

/* Reads the memory map */
void read_memory_map(void) {
    uint8_t i2c_packet[] = {0, 0};
    TWI_read(EEPROM_ADDR, i2c_packet, MEMORY_MAP_SIZE_BYTES);
    fs.memory_map = *(uint64_t *)I2C_get_rx_Buffer();
}

/* Writes the memory map */
void write_memory_map(void) {
    uint8_t i2c_packet[9] = {0};
    for (uint8_t i = 0; i < MEMORY_MAP_SIZE_BYTES; i++)
        i2c_packet[i + 1] = (fs.memory_map >> (i * 8)) & 0xFF;
    TWI_write(EEPROM_ADDR, MEMORY_MAP_SIZE_BYTES + 1, i2c_packet);
    delay_ms(5);
}

/* Converts preset position to a actual pointer in memory. Returns -1 if preset isn't in memory map. */
uint16_t preset_get_mem_pos(uint8_t position) {
    if (position > MAX_SAVES)
        return 0xFFFF;
    if (fs.memory_map & (1UL << position))
        return position * sizeof(PRESET) + MEMORY_MAP_SIZE;
    return 0xFFFF;
}

/* Checks the memory map and counts saves. */
void get_num_of_saves(void) {
    uint8_t stevec = 0;
    for (uint8_t i = 0; i < MAX_SAVES; i++) {
        if (fs.memory_map & (1UL << i))
            stevec++;
    }
    fs.num_of_saves = stevec;
}

/* Loads the preset from 0-MAX_SAVES, returns 0 if it fails, 1 on success*/
uint8_t read_memory(PRESET *preset, uint8_t position) {
    uint16_t mem_position = preset_get_mem_pos(position);

    /*if (mem_position == 0xFFFF)
        return 0;*/
    uint8_t i2c_packet[2] = {mem_position >> 8, mem_position & 0xFF};
    TWI_read(EEPROM_ADDR, i2c_packet, sizeof(PRESET));
    uint8_t *p_to_preset = (uint8_t *)preset;
    uint8_t *p_to_buf = I2C_get_rx_Buffer();
    for (uint16_t i = 0; i < sizeof(PRESET); i++)
        *(p_to_preset++) = *(p_to_buf++);
    return 1;
}

/* Deletes the preset. */
void preset_delete(PRESET *preset) {
    fs.memory_map &= ~(1UL << preset->position);
    write_memory_map();
}
void memory_full_format(void) {
    uint8_t null_array[66];
    for (uint8_t i = 0; i < 66; i++)
        null_array[i] = 0;
    for (uint16_t i = 0; i < MEMORY_SIZE; i += 64) {
        null_array[0] = (i >> 8) & 0xFF;
        null_array[1] = i & 0xFF;
        TWI_write(EEPROM_ADDR, 66, null_array);
        delay_ms(5);
    }
}
void memory_format(void) {
    fs.memory_map = 0;
    write_memory_map();
}
void write_memory(PRESET *preset, uint8_t position) {
    volatile uint16_t remaining_size = sizeof(PRESET);

    uint8_t send_packet[66];
    preset->position = position;
    fs.memory_map |= (1UL << position);
    write_memory_map();

    uint8_t *p_to_preset = (uint8_t *)preset;
    uint16_t memory_position = preset_get_mem_pos(position);
    while (remaining_size > 0) {
        send_packet[0] = (memory_position) >> 8;
        send_packet[1] = (memory_position) & 0xFF;

        if (remaining_size > 64) {
            for (uint16_t i = 2; i < 66; i ++) {
                *(send_packet + i) = *(p_to_preset + i);
            }
            remaining_size -= 64;
            memory_position += 64;
            TWI_write(EEPROM_ADDR, send_packet, 66);
        }
        else {
            //possible index out of range bug
            for (uint16_t i = 2; i < remaining_size + 2; i++) {
                *(send_packet + i) = *(p_to_preset + i);
            }
            remaining_size = 0;
            TWI_write(EEPROM_ADDR, send_packet, remaining_size + 2);
        }
        delay_ms(5);
    }
    /*
    uint8_t *p_to_preset = (uint8_t *)preset;
    while(remaining_size > 0) {
        send_packet[0] = (preset->position + sizeof(PRESET) - remaining_size) >> 8;
        send_packet[1] = (preset->position + sizeof(PRESET) - remaining_size) & 0xFF;
        if(remaining_size > 64)            {
            for(uint16_t i = 2; i < 66; i++)
                *(send_packet + i) = *(p_to_preset++);
            remaining_size -= 64;
            TWI_write(EEPROM_ADDR, send_packet, 66);
        }
        else {
            for(uint16_t i = 2; i < remaining_size; i++)
                *(rec_buffer + i) = *(p_to_preset++);
            remaining_size = 0;
            TWI_write(EEPROM_ADDR, send_packet, remaining_size + 2);
        }
        delay_ms(5);
    }*/
    return;
}