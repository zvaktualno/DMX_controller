#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "my_i2c.h"
#include "i2c_fs.h"


I2C_FS fs;
/* Gets all saves. */
void memory_init(void) {
    fs.memory_map = 0;
    read_memory_map();
    get_num_of_saves();
}

/* Reads the memory map */
void read_memory_map(void) {
    uint8_t i2c_packet[2] = {0, 0};
    i2c_read(EEPROM_ADDR, i2c_packet, MEMORY_MAP_SIZE_BYTES);
    fs.memory_map = *(uint32_t *)i2c_get_rx_Buffer();
}

/* Writes the memory map */
void write_memory_map(void) {
    uint8_t i2c_packet[10] = {0, 0};
    for (uint8_t i = 0; i < MEMORY_MAP_SIZE_BYTES; i++)
        i2c_packet[i + 2] = (fs.memory_map >> (i * 8)) & 0xFF;
    i2c_write(EEPROM_ADDR, i2c_packet, MEMORY_MAP_SIZE_BYTES + 2);
}

/* Converts preset position to a actual pointer in memory. Returns -1 if preset isn't in memory map. */
uint16_t preset_get_mem_pos(uint8_t position) {
    if (position > MAX_SAVES)
        return 0xFFFF;

    uint16_t mem_pos = position * sizeof(PRESET) + MEMORY_PAGE_SIZE;

    if(mem_pos % MEMORY_PAGE_SIZE)
        mem_pos += MEMORY_PAGE_SIZE - mem_pos % 64;

    return mem_pos;
}

/* Checks the memory map and counts saves. */
void get_num_of_saves(void) {
    uint8_t stevec = 0;
    for (uint8_t i = 0; i < MAX_SAVES; i++) {
        if (fs.memory_map & (1ULL << i))
            stevec++;
    }
    fs.num_of_saves = stevec;
}

/* Loads the preset from 0-MAX_SAVES, returns 0 if it fails, 1 on success*/
uint8_t read_memory(PRESET *preset, uint8_t position) {

    if(!(fs.memory_map & (1 << position)))
        return 0;
    uint16_t mem_position = preset_get_mem_pos(position);

    if (mem_position == 0xFFFF)
        return 0;
    uint8_t i2c_packet[2] = {mem_position >> 8, mem_position & 0xFF};
    i2c_read(EEPROM_ADDR, i2c_packet, sizeof(PRESET));
    uint8_t *p_to_preset = (uint8_t *)preset;
    uint8_t *p_to_buf = i2c_get_rx_Buffer();
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
        null_array[0] = i >> 8;
        null_array[1] = i & 0xFF;
        i2c_write(EEPROM_ADDR, null_array, 66);
    }
    read_memory_map();
}

void memory_format(void) {
    fs.memory_map = 0;
    write_memory_map();
}

void write_memory(PRESET *preset, uint8_t position) {
    volatile uint16_t remaining_size = sizeof(PRESET);

    uint8_t send_packet[66];
    preset->position = position;
    fs.memory_map |= (1ULL << position);
    write_memory_map();

    uint8_t *p_to_preset = (uint8_t *)preset;
    uint16_t memory_position = preset_get_mem_pos(position);

    for(uint16_t i = 0; i < sizeof(PRESET); i += 64) {
        send_packet[0] = (memory_position) >> 8;
        send_packet[1] = (memory_position) & 0xFF;

        if(remaining_size > 64) {
            for(uint8_t j = 0; j < 64; j++) {
                send_packet[j + 2] = *(p_to_preset++);
                memory_position++;
                remaining_size--;
            }
            i2c_write(EEPROM_ADDR, send_packet, 66);
        }

        if(remaining_size < 64 && remaining_size > 0) {
            send_packet[0] = (memory_position) >> 8;
            send_packet[1] = (memory_position) & 0xFF;
            for(uint8_t j = 0; j < remaining_size; j++) {
                send_packet[j + 2] = *(p_to_preset++);
            }
            i2c_write(EEPROM_ADDR, send_packet, remaining_size);
            break;
        }
    }

    return;
}

void memory_load_preset(channel **ch, uint8_t *dmx_ch, uint8_t pos) {
    PRESET preset;
    if(!read_memory(&preset, pos))return;
    for(uint16_t i = 0; i < 256; i++) {
        *(dmx_ch + i) = *(preset.channels + i);
    }
    for(uint8_t i = 0; i < 5; i++)    {
        ch[i]->level = preset.level[i];
        ch[i]->enabled = preset.enabled[i];
        ch[i]->dmx_ch = preset.dmx_ch[i];
        ch[i]->input_channel = preset.input_channel[i];
        ch[i]->midi_ch = preset.midi_ch[i];
        ch[i]->note = preset.note[i];
        ch[i]->adsr->attack = preset.attack[i];
        ch[i]->adsr->decay = preset.decay[i];
        ch[i]->adsr->sustain = preset.sustain[i];
        ch[i]->adsr->sustain_level = preset.sustain_level[i];
        ch[i]->adsr->release = preset.release[i];
    }
}

void memory_write_preset(channel **ch, uint8_t *dmx_ch, uint8_t pos) {
    PRESET preset;

    for(uint16_t i = 0; i < 256; i++) {
        *(preset.channels + i) = *(dmx_ch + i);
    }
    for(uint8_t i = 0; i < 5; i++)    {
        preset.level[i] = ch[i]->level;
        preset.enabled[i] = ch[i]->enabled;
        preset.dmx_ch[i] = ch[i]->dmx_ch;
        preset.input_channel[i] = ch[i]->input_channel;
        preset.midi_ch[i] = ch[i]->midi_ch;
        preset.note[i] = ch[i]->note;
        preset.attack[i] = ch[i]->adsr->attack;
        preset.decay[i] = ch[i]->adsr->decay;
        preset.sustain[i] = ch[i]->adsr->sustain;
        preset.sustain_level[i] = ch[i]->adsr->sustain_level;
        preset.release[i] = ch[i]->adsr->release;
    }

    write_memory(&preset, pos);
}
void memory_get_preset_name(char *dest, uint8_t num) {
    PRESET preset;
    if(num == 0) {
        sprintf(dest, "");
        return;
    }
    if(!read_memory(&preset, num))
        sprintf(dest, "EMPTY %d", num);
    else
        sprintf(dest, "SAVE %d", num);
    return;
}