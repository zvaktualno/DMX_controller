#ifndef I2C_FS_H_
#define I2C_FS_H_

#define EEPROM_ADDR 0b1010000
#define MAX_SAVES 32
#define MEMORY_SIZE 0x3FFF
#define MEMORY_PAGE_SIZE 64
#define MEMORY_MAP_SIZE 64
#define MEMORY_MAP_SIZE_BYTES 8

typedef struct {
    uint32_t memory_map;
    uint8_t num_of_saves;
} I2C_FS;

void memory_init(void);
void read_memory_map(void);
void write_memory_map(void);

uint16_t preset_get_mem_pos(uint8_t position);
void get_num_of_saves(void);
void memory_full_format(void);
uint8_t read_memory(PRESET *preset, uint8_t position);
void write_memory(PRESET *preset, uint8_t position);
void preset_delete(PRESET *preset);
#endif /* I2C_FS_H_ */