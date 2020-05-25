
#include <asf.h>
#include <stdint.h>
#include "adsr.h"
#include "timers.h"
#include "my_adc.h"
#include "my_USB.h"
#include "RRW_eeprom.h"
#include "my_i2c.h"
#include "dmx_lib.h"

ADSR channel0;
ADSR channel1;
ADSR channel2;
ADSR channel3;
ADSR channel4;

typedef enum {
    TRIGGER,
    DMX_SEND,
    BOTH
} MODE;
MODE device_mode;
int main (void) {
    system_init();
    uint8_t dmx_channels[DMX_UNIVERSE_SIZE];
    for(uint16_t i = 0; i < 256; i++) {
        dmx_channels[i] = i;
    }
    /*configure_tcc0();
    configure_tcc0_callbacks(&channel0, &channel1, &channel2, &channel3, &channel4);*/
    /*configure_adc0();
    configure_adc0_callbacks();*/
    configure_USB();
    configure_USB_callbacks();
    device_mode = TRIGGER;
    uint8_t test = 0;
    while(1) {
        test++;
        if(test % 20 > 10)
            usart_change_baudrate(250000);
        else
            usart_change_baudrate(9600);
        if(device_mode == DMX_SEND || device_mode == BOTH) {
            BREAKPOINT;
            DMX_Send(dmx_channels);
        }
        if(device_mode == TRIGGER || device_mode == BOTH) {
            BREAKPOINT;
            uint8_t dmx_values[] = {1, 2, 3, 4, 5};
            USB_SendMessage(dmx_channels, 256);
        }
        delay_ms(333);
    }
}