
#ifndef _REC_EEPROM_
#define _REC_EEPROM_

#include "config.h"
typedef struct {
    char a;
} E_SETTINGS;
/**

 * The \c NVMCTRL_FUSES_EEPROM_SIZE has to be set to 0x00 in the fuse setting
 * of the device to run this test. Atmel Studio can be used to set this fuse
 * (Tools->Device Programming).
 */

void configure_eeprom(void);
void single_write_eeprom(uint16_t mem_pos, uint8_t st_eeproma, uint8_t *p, uint16_t size);
void beri_EEPROM(E_SETTINGS *read_eeprom, uint8_t st_eeproma);
void write_whole_eeprom (E_SETTINGS *p, uint8_t st_eeproma);
void read_whole_eeprom(E_SETTINGS *p, uint8_t st_eeproma);
/* Structure for UART module connected to EDBG (used for unit test output) */
struct usart_module cdc_uart_module;
/* Structure to store EEPROM emulator parameters */
struct rww_eeprom_emulator_parameters ee_params;
void check_eeprom(E_SETTINGS *ptr1, E_SETTINGS *ptr2, E_SETTINGS *ptr3);
void single_write_all_eeprom(uint16_t mem_pos, uint8_t *p, uint16_t size);
#endif
