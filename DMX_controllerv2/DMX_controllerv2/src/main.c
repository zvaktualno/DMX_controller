
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
MENU main_menu = {.num_of_items = 0;};
MENU channel1_menu = {.num_of_items = 0;};
MENU channel2_menu = {.num_of_items = 0;};
MENU channel3_menu = {.num_of_items = 0;};
MENU channel4_menu = {.num_of_items = 0;};
MENU channel5_menu = {.num_of_items = 0;};

channel dmx_ch1;
channel dmx_ch2;
channel dmx_ch3;
channel dmx_ch4;
channel dmx_ch5;


MENU *selected_menu;

int main (void) {
    system_init();
    uint8_t dmx_values[500];

    ADSR channel0 = {.p_to_var = dmx_values + 0};
    ADSR channel1 = {.p_to_var = dmx_values + 1};
    ADSR channel2 = {.p_to_var = dmx_values + 2};
    ADSR channel3 = {.p_to_var = dmx_values + 3};
    ADSR channel4 = {.p_to_var = dmx_values + 4};


    channel *p_to_channels[5] = {&dmx_ch1, &dmx_ch2, &dmx_ch3, &dmx_ch4, &dmx_ch5};
    MENU *p_to_menus[5] = {&channel1_menu, &channel2_menu, &channel3_menu, &channel4_menu, &channel5_menu};

    uint8_t brightness  = 0, contrast = 0;
    menu_item tmp_item;
    menu_create_item(&tmp_item, "BRIGHTNES", UINT8, "", (void *)&brightness);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CONTRAST", UINT8, "", (void *)&contrast);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 1", TYPE_MENU, "", (void *)&p_to_menus[0]);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 2", TYPE_MENU, "", (void *)&p_to_menus[1]);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 3", TYPE_MENU, "", (void *)&p_to_menus[2]);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 4", TYPE_MENU, "", (void *)&p_to_menus[3]);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 5", TYPE_MENU, "", (void *)&p_to_menus[4]);
    menu_add_item(&main_menu, tmp_item);

    for(uint8_t i = 0; i < sizeof(p_to_channels); i++) {
        menu_create_item(&tmp_item, "DMX CHAN", UINT32, "", (void *)&p_to_channels[i]->ch);
        menu_add_item(&p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "LEVEL", FLOAT, "", (void *)&p_to_channels[i]->level);
        menu_add_item(&p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "ATTACK", UINT32, "", (void *)&p_to_channels[i]->A);
        menu_add_item(&p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "DECAY", UINT32, "", (void *)&p_to_channels[i]->D);
        menu_add_item(&p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "SUSTAIN", UINT32, "", (void *)&p_to_channels[i]->S);
        menu_add_item(&p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "RELEASE", UINT32, "", (void *)&p_to_channels[i]->R);
        menu_add_item(&p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu);
        menu_add_item(&p_to_menus[i], tmp_item);
    }


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