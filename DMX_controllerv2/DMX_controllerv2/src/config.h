
#ifndef CONFIG_H_
#define CONFIG_H_
#include "adsr.h"

#define ADC0_RESOLUTION ADC_RESOLUTION_8BIT

#define BREAKPOINT asm("nop")

#define TWI_BUFFER_SIZE 64

#define USB_TX_PINMUX 255
#define USB_RX_PINMUX 255
#define USB_MODULE SERCOM1

/* DMX UART definitions */
#define DMX_TX_PINMUX 255
#define DMX_RX_PINMUX 255
#define DMX_MODULE SERCOM5
#define MAX_DMX_CHANNELS 256

/* I2C definitions */
#define I2C_MODULE SERCOM3
#define I2C_PADS
#define I2C_SCL_PINMUX PINMUX_PA23D_SERCOM5_PAD1
#define I2C_SDA_PINMUX PINMUX_PA22D_SERCOM5_PAD0
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

#define PIN_SW1 PIN_PA12
#define PIN_SW1_EIC_LINE 12

#define PIN_SW2 PIN_PA13
#define PIN_SW2_EIC_LINE 13

#define PIN_ADC0 PIN_PA04
#define PIN_ADC1 PIN_PA05
#define PIN_ADC2 PIN_PA06
#define PIN_ADC3 PIN_PA07
#define PIN_ADC4 PIN_PA03
#define PIN_LCD_VO PIN_PA02

/* MUX definitions */
#define MUX_ADC0 PINMUX_PA04B_ADC0_AIN4
#define MUX_ADC1 PINMUX_PA05B_ADC0_AIN5
#define MUX_ADC2 PINMUX_PA06B_ADC0_AIN6
#define MUX_ADC3 PINMUX_PA07B_ADC0_AIN7
#define MUX_ADC4 PINMUX_PA03B_ADC0_AIN1
#define MUX_LCD_VO PINMUX_PA02B_DAC_VOUT

typedef struct {
    uint32_t ch, A, D, S, R;
    float level;
} channel;
typedef enum {
    SCROLL,
    EDIT,
} STATE ;
typedef struct {
    char preset_name[10];
    uint16_t position;
    uint8_t channels[512];
    channel ch;
} PRESET;

typedef enum {
    BUTTON_NONE,
    BUTTON_1,
    BUTTON_2,
} TIPKA;

#endif