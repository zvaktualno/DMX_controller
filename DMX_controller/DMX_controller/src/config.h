
#ifndef CONFIG_H_
#define CONFIG_H_
#include "adsr.h"

#define BREAKPOINT asm("nop")

#define TWI_BUFFER_SIZE 64

#define USB_MODULE SERCOM1

/* DMX UART definitions */

#define DMX_MODULE SERCOM5
#define DMX_IRQn SERCOM5_IRQn
#define MAX_DMX_CHANNELS 256

/* I2C definitions */
#define I2C_MODULE SERCOM3
#define I2C_SCL_PINMUX PINMUX_PA23C_SERCOM3_PAD1
#define I2C_SDA_PINMUX PINMUX_PA22C_SERCOM3_PAD0
#define I2C_BAUDRATE I2C_MASTER_BAUD_RATE_400KHZ
#define I2C_TIMEOUT 10000

/* PIN definitions */
#define PIN_ENC_A PIN_PB23
#define PIN_ENC_B PIN_PB22
#define PIN_EEPROM_WP PIN_PA24
#define PIN_LED_POWER PIN_PA28
#define PIN_LCD_D0 PIN_PB06
#define	PIN_LCD_D1 PIN_PB05
#define	PIN_LCD_D2 PIN_PB04
#define	PIN_LCD_D3 PIN_PB03
#define	PIN_LCD_D4 PIN_PB02
#define	PIN_LCD_D5 PIN_PB01
#define PIN_LCD_D6 PIN_PB00
#define PIN_LCD_D7 PIN_PB31
#define PIN_LCD_EN PIN_PB07
#define PIN_LCD_RW PIN_PB08
#define PIN_LCD_RS PIN_PB09
#define PIN_LCD_BL PIN_PB11
#define PIN_DMX_DIR PIN_PA20
#define PIN_SW1 PIN_PA12
#define PIN_SW1_EIC_LINE 12
#define TX_PIN PIN_PB16


#define PIN_SW2 PIN_PA13
#define PIN_SW2_EIC_LINE 13

#define PIN_ADC0 PIN_PA03
#define PIN_ADC1 PIN_PA04
#define PIN_ADC2 PIN_PA05
#define PIN_ADC3 PIN_PA06
#define PIN_ADC4 PIN_PA07
#define PIN_LCD_VO PIN_PA02

/* MUX definitions */

#define MUX_ADC0 MUX_PA03B_AC_AIN5
#define MUX_ADC1 MUX_PA04B_AC_AIN0
#define MUX_ADC2 MUX_PA05B_AC_AIN1
#define MUX_ADC3 MUX_PA06B_ADC0_AIN6
#define MUX_ADC4 MUX_PA07B_ADC0_AIN7


#define MUX_LCD_VO PINMUX_PA02B_DAC_VOUT
#define LCD_ARROW_CHAR 7
typedef struct {
    uint8_t level;
    uint8_t enabled;
    uint8_t dmx_ch;
    uint8_t input_channel;
    uint8_t midi_ch;
    uint8_t note;
    ADSR *adsr;
    uint8_t triggered;
} channel;

typedef enum {
    SCROLL,
    EDIT,
} STATE ;



typedef enum {
    BUTTON_NONE,
    BUTTON_1,
    BUTTON_2,
} TIPKA;

typedef struct {
    uint8_t contrast;
    uint8_t brightness;
    uint8_t fixture_size;
    uint8_t mode;
} SETTINGS;

typedef struct {
    uint16_t position;
    uint8_t channels[256];
    uint8_t level[5];
    uint8_t enabled[5];
    uint8_t dmx_ch[5];
    uint8_t input_channel[5];
    uint8_t midi_ch[5];
    uint8_t note[5];
    uint32_t attack[5];
    uint32_t decay[5];
    uint32_t sustain[5];
    uint32_t sustain_level[5];
    uint32_t release[5];
} PRESET;

#endif