#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "tipke.h"

uint8_t previous_button_state =0;

TIPKA button_read(void)
{
    uint8_t button_state = (port_pin_get_input_level(PIN_SW2)<<1) | (!port_pin_get_input_level(PIN_SW1)<<0);

    uint8_t pressed_keys = (button_state ^ previous_button_state) & button_state;
    uint8_t released_keys = (button_state ^ previous_button_state) & previous_button_state;
    previous_button_state=button_state;
    if(pressed_keys & (1<<0)) {
        return BUTTON_1;
    }
    if(pressed_keys & (1<<1)) {
        return BUTTON_2;
    }

    pressed_keys &=~(released_keys);
    return BUTTON_NONE;

    /*
       if((button_state>>0)&1) {
           button_state &= ~(1<<0);
           return BUTTON_1;
       }
    if((button_state>>1)&1) {
        button_state &=~(1<<1);
        return BUTTON_2;
    }
    return BUTTON_NONE;*/
}