
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

#define ENC_A_PIN 50
#define ENC_B_PIN 49
#define EEPROM_WP_PIN 45
#define LED_POWER_PIN 53
#define LCD_D0_PIN 9
#define	LCD_D1_PIN 6
#define	LCD_D2_PIN 5
#define	LCD_D3_PIN 64
#define	LCD_D4_PIN 63
#define	LCD_D5_PIN 62
#define LCD_D6_PIN 61
#define LCD_D7_PIN 60
#define LCD_EN_PIN 10
#define LCD_RW_PIN 12
#define LCD_RS_PIN 1

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


#endif