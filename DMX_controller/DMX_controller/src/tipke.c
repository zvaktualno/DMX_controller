#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "tipke.h"
uint8_t button_state=0;
uint8_t previous_button_state =0;

void configure_extint_channel(void)
{
    //! [setup_1]
    /* struct extint_chan_conf config_extint_chan;

     extint_chan_get_config_defaults(&config_extint_chan);

     config_extint_chan.gpio_pin           = PIN_SW1;
     config_extint_chan.gpio_pin_mux       = PINMUX_PA12A_EIC_EXTINT12;
     config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;
     config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;

     extint_chan_set_config(PIN_SW1_EIC_LINE, &config_extint_chan);

     config_extint_chan.gpio_pin           = PIN_SW2;
     config_extint_chan.gpio_pin_mux       = PINMUX_PA13A_EIC_EXTINT13;
     config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;
     config_extint_chan.detection_criteria = EXTINT_DETECT_RISING;

     extint_chan_set_config(PIN_SW2_EIC_LINE, &config_extint_chan);
     configure_extint_callbacks();*/
}

void configure_extint_callbacks(void)
{
    /* extint_register_callback(extint_detection_callback, PIN_SW1_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
     extint_register_callback(extint_detection_callback, PIN_SW2_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);

     extint_chan_enable_callback(PIN_SW1_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
     extint_chan_enable_callback(PIN_SW2_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);*/

}

void extint_detection_callback(void)
{
    //button_state |= (port_pin_get_input_level(PIN_SW2)<<1)|(!port_pin_get_input_level(PIN_SW1)<<0);
}

TIPKA button_read(void)
{
    button_state = (port_pin_get_input_level(PIN_SW2)<<1) | (!port_pin_get_input_level(PIN_SW1)<<0);

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