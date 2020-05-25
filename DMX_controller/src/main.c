
#include <asf.h>
#include <stdint.h>
#include "adsr.h"
#include "timers.h"
#include "my_adc.h"
#include "my_USB.h"
#include "RRW_eeprom.h"
#include "my_i2c.h"

ADSR channel0;
ADSR channel1;
ADSR channel2;
ADSR channel3;
ADSR channel4;

typedef enum {
    TRIGGER,
    DMX,
    BOTH
} MODE;
MODE device_mode;
int main (void) {
    system_init();
    /*configure_tcc0();
    configure_tcc0_callbacks(&channel0, &channel1, &channel2, &channel3, &channel4);*/
    /*configure_adc0();
    configure_adc0_callbacks();*/
    configure_USB();
    configure_USB_callbacks();
    device_mode = TRIGGER;
    while(1) {
        if(device_mode == DMX || device_mode == BOTH) {
            BREAKPOINT;
            //send_DMX();
        }
        if(device_mode == TRIGGER || device_mode == BOTH) {
            BREAKPOINT;
            uint8_t dmx_values[] = {1, 2, 3, 4, 5};
            USB_SendMessage(dmx_values, 5);
        }
        delay_ms(2000);
    }
}