
#include <asf.h>
#include <stdint.h>
#include "adsr.h"
#include "timers.h"
#include "my_adc.h"
#include "USB.h"
#include "DMX.h"
#include "RRW_eeprom.h"
#include "my_i2c.h"


typedef enum {
    TRIGGER,
    DMX,
    BOTH
} MODE;
MODE device_mode;


void NMI_Handler(void) {
    BREAKPOINT;
}
void HardFault_Handler(void) {
    BREAKPOINT;
}
void SVC_Handler(void) {
    BREAKPOINT;
}
void PendSV_Handler(void) {
    BREAKPOINT;
}
void SysTick_Handler(void) {
    BREAKPOINT;
}

int main (void) {
    system_init();
    uint8_t dmx_values[500];

    ADSR channel0 = {.p_to_var = dmx_values + 0};
    ADSR channel1 = {.p_to_var = dmx_values + 1};
    ADSR channel2 = {.p_to_var = dmx_values + 2};
    ADSR channel3 = {.p_to_var = dmx_values + 3};
    ADSR channel4 = {.p_to_var = dmx_values + 4};

    configure_tcc0();
    configure_tcc0_callbacks(&channel0, &channel1, &channel2, &channel3, &channel4);

    for(uint16_t i = 0; i < sizeof(dmx_values); i++) {
        dmx_values[i] = 0;
    }

    /*configure_adc0();
    configure_adc0_callbacks();*/
    configure_USB();
    configure_DMX();

    device_mode = TRIGGER;
    uint32_t send_data_timer = 0;
    while(1) {
        if(millis() - send_data_timer > 100) {
            send_data_timer = millis();

            if(device_mode == DMX || device_mode == BOTH) {
                BREAKPOINT;
                DMX_SendMessage(dmx_values, sizeof(dmx_values));
            }

            if(device_mode == TRIGGER || device_mode == BOTH) {
                BREAKPOINT;
                uint8_t dmx_values[] = {get_ADSR_value(&channel0), get_ADSR_value(&channel1), get_ADSR_value(&channel2), get_ADSR_value(&channel3), get_ADSR_value(&channel4)};
                USB_SendMessage(dmx_values, 5);
            }
        }
    }
}