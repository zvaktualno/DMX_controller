
#ifndef I2C_FS_H_
#define I2C_FS_H_

#define EEPROM_ADDR 0xFF
#define MAX_SAVES 32

typedef struct {
    uint32_t memory_map;
    uint8_t num_of_saves;
} I2C_FS;

void read_memory_map(I2C_FS *fs);
void write_memory_map(I2C_FS *fs);

#endif /* I2C_FS_H_ */