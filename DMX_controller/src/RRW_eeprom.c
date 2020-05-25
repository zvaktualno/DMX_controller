#include <asf.h>
#include <string.h>
#include "RRW_eeprom.h"
#include "config.h"
#include "my_i2c.h"
/**

 * The \c NVMCTRL_FUSES_EEPROM_SIZE has to be set to 0x00 in the fuse setting
 * of the device to run this test. Atmel Studio can be used to set this fuse
 * (Tools->Device Programming).
 */

void configure_eeprom(void) {
    /* Setup EEPROM emulator service */
    enum status_code error_code = rww_eeprom_emulator_init();
    if (error_code == STATUS_ERR_NO_MEMORY) {
        while (true) {
            /* No EEPROM section has been set in the device's fuses */
        }
    }
    else
        if (error_code != STATUS_OK) {
            /* Erase the emulated EEPROM memory (assume it is unformatted or
             * irrecoverably corrupt) */
            rww_eeprom_emulator_erase_memory();
            rww_eeprom_emulator_init();
        }
}

void beri_EEPROM(E_SETTINGS *eeprom_struct, uint8_t st_eeproma) {

    read_whole_eeprom(eeprom_struct, st_eeproma);
    uint16_t eeprom_offset = st_eeproma;
}

void write_whole_eeprom (E_SETTINGS *p, uint8_t st_eeproma) {

}

void read_whole_eeprom(E_SETTINGS *p,  uint8_t st_eeproma) {
    volatile uint16_t eeprom_offset = st_eeproma;

}
void single_write_eeprom(uint16_t mem_pos, uint8_t st_eeproma, uint8_t *p, uint16_t size) {
    uint16_t eeprom_offset = st_eeproma ;
    rww_eeprom_emulator_write_buffer(mem_pos + eeprom_offset, p, size);
    rww_eeprom_emulator_commit_page_buffer();
}

void single_write_all_eeprom(uint16_t mem_pos, uint8_t *p, uint16_t size) {
    single_write_eeprom(mem_pos, 0, p, size);
    single_write_eeprom(mem_pos, 1, p, size);
    single_write_eeprom(mem_pos, 2, p, size);
}
