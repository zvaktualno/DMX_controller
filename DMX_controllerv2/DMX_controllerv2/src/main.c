
#include <asf.h>
#include <stdint.h>
#include "adsr.h"
#include "timers.h"
#include "my_adc.h"
#include "USB.h"
#include "DMX.h"
#include "RRW_eeprom.h"
#include "my_i2c.h"
#include "my_menu.h"

typedef enum {
    TRIGGER,
    DMX,
    BOTH
} MODE;

MODE device_mode;
MODE select_device_mode(uint8_t mode);

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
MENU main_menu = {.num_of_items = 0};
MENU channel1_menu = {.num_of_items = 0};
MENU channel2_menu = {.num_of_items = 0};
MENU channel3_menu = {.num_of_items = 0};
MENU channel4_menu = {.num_of_items = 0};
MENU channel5_menu = {.num_of_items = 0};
MENU settings_menu = {.num_of_items = 0};
MENU static_channels_menu = {.num_of_items = 0};

channel dmx_ch1;
channel dmx_ch2;
channel dmx_ch3;
channel dmx_ch4;
channel dmx_ch5;


MENU *selected_menu = &main_menu;

int main (void) {
    system_init();
    uint8_t dmx_values[500];

    ADSR adsr_channel0 = {.p_to_var = dmx_values + 0};
    ADSR adsr_channel1 = {.p_to_var = dmx_values + 1};
    ADSR adsr_channel2 = {.p_to_var = dmx_values + 2};
    ADSR adsr_channel3 = {.p_to_var = dmx_values + 3};
    ADSR adsr_channel4 = {.p_to_var = dmx_values + 4};
    uint8_t device_mode_num = 0;
    MENU *p_to_static_dmx_channels[16];
    for(uint8_t i = 0; i < 16; i++) {
        p_to_static_dmx_channels[i] = malloc(sizeof(MENU));
    }
    channel *p_to_channels[5] = {&dmx_ch1, &dmx_ch2, &dmx_ch3, &dmx_ch4, &dmx_ch5};
    MENU *p_to_menus[5] = {&channel1_menu, &channel2_menu, &channel3_menu, &channel4_menu, &channel5_menu};

    uint8_t brightness  = 0, contrast = 0;
    menu_item tmp_item;

    menu_create_item(&tmp_item, "CHANNEL 1", TYPE_MENU, "", (void *)&p_to_menus[0], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 2", TYPE_MENU, "", (void *)&p_to_menus[1], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 3", TYPE_MENU, "", (void *)&p_to_menus[2], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 4", TYPE_MENU, "", (void *)&p_to_menus[3], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 5", TYPE_MENU, "", (void *)&p_to_menus[4], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "SETTINGS", TYPE_MENU, "", (void *)&settings_menu, 0, 0);
    menu_add_item(&main_menu, tmp_item);

    menu_create_item(&tmp_item, "BRIGHTNES", UINT8, "", (void *)&brightness, 0, 0);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "CONTRAST", UINT8, "", (void *)&contrast, 0, 0);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "MODE", UINT8, "", (void *)&device_mode_num, 0, 3);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0);
    menu_add_item(&settings_menu, tmp_item);

    for(uint16_t i = 0; i < (MAX_DMX_CHANNELS / 16); i++) {
        char menu_item_name[10];

        for(uint16_t j = 0; j < MENU_MAX_ITEMS ; j++) {
            sprintf(menu_item_name, "DMX%d", i * MENU_MAX_ITEMS + j);
            menu_create_item(&tmp_item, menu_item_name, UINT8, "", dmx_values + i * 16 + j, 0, 256);
            menu_add_item(p_to_static_dmx_channels[i], tmp_item);
        }
        menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&static_channels_menu, 0, 0);
        menu_add_item(p_to_static_dmx_channels[i], tmp_item);

        sprintf(menu_item_name, "DMX%d-%d", i * 16, i * 16 + 15);
        menu_create_item(&tmp_item, menu_item_name, TYPE_MENU, "", (void *)&p_to_static_dmx_channels[i], 0, 0);
        menu_add_item(&static_channels_menu, tmp_item);
    }
    menu_create_item(&tmp_item, "STATIC_CH", TYPE_MENU, "", (void *)&static_channels_menu, 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0);
    menu_add_item(&static_channels_menu, tmp_item);

    for(uint8_t i = 0; i < 5; i++) {
        menu_create_item(&tmp_item, "DMX CH", UINT32, "", (void *)&p_to_channels[i]->ch, 0, 255);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "LEVEL", FLOAT, "", (void *)&p_to_channels[i]->level, 0, 255);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "ATTACK", UINT32, "ms", (void *)&p_to_channels[i]->A, 0, 0);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "DECAY", UINT32, "ms", (void *)&p_to_channels[i]->D, 0, 0);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "SUSTAIN", UINT32, "", (void *)&p_to_channels[i]->S, 0, 0);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "RELEASE", UINT32, "ms", (void *)&p_to_channels[i]->R, 0, 0);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0);
        menu_add_item(p_to_menus[i], tmp_item);
    }

    /*
        configure_tcc0();
        configure_tcc0_callbacks(&adsr_channel0, &adsr_channel1, &adsr_channel2, &adsr_channel3, &adsr_channel4);
    */
    for(uint16_t i = 0; i < sizeof(dmx_values); i++) {
        dmx_values[i] = 0;
    }

    /*configure_adc0();
    configure_adc0_callbacks();*/
    /*  configure_USB();
      configure_DMX();

      device_mode = TRIGGER;
      uint32_t send_data_timer = 0;*/
    while(1) {
        delay_ms(1000);
        increment_menu_position(selected_menu);
    }
    /*
      while(1) {
          if(millis() - send_data_timer > 1000) {
              send_data_timer = millis();
              device_mode = select_device_mode(device_mode_num);
              if(device_mode == DMX || device_mode == BOTH) {
                  BREAKPOINT;
                  DMX_SendMessage(dmx_values, sizeof(dmx_values));
              }

              if(device_mode == TRIGGER || device_mode == BOTH) {
                  BREAKPOINT;
                  uint8_t usb_values[] = {get_ADSR_value(&adsr_channel0), get_ADSR_value(&adsr_channel1), get_ADSR_value(&adsr_channel2), get_ADSR_value(&adsr_channel3), get_ADSR_value(&adsr_channel4)};
                  USB_SendMessage(usb_values, 5);
              }
          }
      }*/
}

MODE select_device_mode(uint8_t mode) {
    switch(device_mode) {
        case 0:
            return DMX;
        case 1:
            return TRIGGER;
        case 2:
            return BOTH;
        default:
            return DMX;
    }
}