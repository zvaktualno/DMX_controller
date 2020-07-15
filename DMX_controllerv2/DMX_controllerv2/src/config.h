
#ifndef CONFIG_H_
#define CONFIG_H_
#include "adsr.h"
#define ADC_PIN 255
#define ADC0_RESOLUTION ADC_RESOLUTION_8BIT

#define BREAKPOINT asm("nop")

#define TWI_SDA_PINMUX 0xFF //PINMUX_PA16C_SERCOM1_PAD0
#define TWI_SCL_PINMUX 0xff	//PINMUX_PA17C_SERCOM1_PAD1
#define TWI_MODULE SERCOM3
#define I2C_TIMEOUT 10000
#define TWI_SDA_PIN PIN_PA16C_SERCOM1_PAD0
#define TWI_SCL_PIN PIN_PA17C_SERCOM1_PAD1
#define TWI_BUFFER_SIZE 64

#define USB_TX_PINMUX 255
#define USB_RX_PINMUX 255
#define USB_MODULE SERCOM4

typedef struct {
    char preset_name[10];
    uint16_t position;
    uint8_t channels[512];
    ADSR preset_data[5];
} PRESET;

#endif