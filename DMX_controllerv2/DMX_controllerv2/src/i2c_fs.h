
#ifndef I2C_FS_H_
#define I2C_FS_H_

#define EEPROM_ADDR 0xFF
#define MAX_SAVES 32
#define MEMORY_SIZE 0x3FF
#define MEMORY_PAGE_SIZE 64
#define MEMORY_MAP_SIZE 64
#define MEMORY_MAP_SIZE_BYTES 8

typedef struct {
    uint32_t memory_map;
    uint8_t num_of_saves;
} I2C_FS;

#include <asf.h>
#include <stdint.h>
#include "i2c_fs.h"
#include "my_i2c.h"
#include "config.h"

void preset_init(void);
void read_memory_map(void);
void write_memory_map(void);

static uint16_t preset_get_mem_pos(uint8_t position);
static void get_num_of_saves(void);

uint8_t read_memory(PRESET *preset, uint8_t position);
void write_memory(PRESET *preset, uint8_t position);
void preset_delete(PRESET *preset);
#endif /* I2C_FS_H_ */