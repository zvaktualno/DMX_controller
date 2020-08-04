
#include <asf.h>
#include <stdint.h>
#include <string.h>
#include "config.h"
#include "MIDI.h"
#include "adsr.h"
#include "timers.h"
#include "my_adc.h"
#include "USB.h"
#include "DMX.h"
#include "my_i2c.h"
#include "my_menu.h"
#include "2004LCD.h"
#include "tipke.h"
#include "i2c_fs.h"

//AC sync bug: https://borkedlabs.com/blog/2017/10-12-samc21-ac-comparator-sync/
typedef enum
{
    TRIGGER,
    DMX,
    BOTH,
} MODE;

MODE device_mode;
MODE select_device_mode(uint8_t mode);
void configure_dac(void);
void configure_dac_channel(void);
uint8_t button_handler(TIPKA t, STATE *s);
void menu_draw(void);
void get_menu_mode_string(char *dest, uint8_t mode);
struct dac_module dac_instance;
void IO_init(void);
void get_menu_bar_string(char *dest, uint8_t value);
void get_menu_enable_string(char *dest, uint8_t mode);
void get_menu_dmx_ch_string(char *dest, uint8_t num);
void get_menu_confirm_string(char *dest, uint8_t num);
void device_factory_reset(void);

void NMI_Handler(void)
{
    BREAKPOINT;
}
void HardFault_Handler(void)
{
    BREAKPOINT;
}
void SVC_Handler(void)
{
    BREAKPOINT;
}
void PendSV_Handler(void)
{
    BREAKPOINT;
}
void SysTick_Handler(void)
{
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
MENU *p_to_menus[5] = {&channel1_menu, &channel2_menu, &channel3_menu, &channel4_menu, &channel5_menu};
MENU *selected_menu = &main_menu;

uint8_t dmx_values[256];

ADSR adsr_channels[5];
channel trigger_channels[5];

STATE state = SCROLL;
SETTINGS device_settings = {.contrast = 8, .brightness = 16};

uint8_t key_pressed = 0;
channel *p_to_channels[5];
uint8_t lsave = 0, ssave = 0, format = 0, fac_reset = 0,dsave=0;

MENU m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15,m16;
int main(void)
{

    system_init();
    delay_init();
    Disable_global_interrupt();
    device_factory_reset();

    configure_i2c();

    configure_ac(trigger_channels);
    IO_init();

    configure_dac();
    configure_dac_channel();
    configure_USB();
    configure_DMX();
    configure_tc1();
    dac_enable(&dac_instance);

    configure_tcc0();
    configure_tcc0_callbacks(adsr_channels);
    configure_adc0(trigger_channels);
    lcd_begin();
    delay_ms(1); //wait for LCD to set up
    lcd_noCursor();
    delay_ms(1);
    lcd_create_bar_charts();
    delay_ms(1);
    lcd_create_arrow();
    NVIC_SetPriority(SERCOM3_IRQn,6);
    NVIC_SetPriority(AC_IRQn, 5);
    NVIC_SetPriority(ADC0_IRQn, 4);
    NVIC_SetPriority(DMX_IRQn, 2);

    Enable_global_interrupt();
    delay_ms(10);
    memory_init();

    MENU *p_to_dmx_group_menus[16]= {&m1,&m2,&m3,&m4,&m5,&m6,&m7,&m8,&m9,&m10,&m11,&m12,&m13,&m14,&m15,&m16};

    menu_item tmp_item;

    menu_create_item(&tmp_item, "SETTINGS", TYPE_MENU, "", (void *)&settings_menu, 0, 0, NULL);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 1", TYPE_MENU, "", (void *)p_to_menus[0], 0, 0, NULL);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 2", TYPE_MENU, "", (void *)p_to_menus[1], 0, 0, NULL);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 3", TYPE_MENU, "", (void *)p_to_menus[2], 0, 0, NULL);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 4", TYPE_MENU, "", (void *)p_to_menus[3], 0, 0, NULL);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 5", TYPE_MENU, "", (void *)p_to_menus[4], 0, 0, NULL);
    menu_add_item(&main_menu, tmp_item);

    menu_create_item(&tmp_item, "CONTRAST", TYPE_BAR, "", (void *)&device_settings.contrast, 0, 16, get_menu_bar_string);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "BRIGHTNESS", TYPE_BAR, "", (void *)&device_settings.brightness, 0, 16, get_menu_bar_string);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "MODE", TYPE_ENUM, "", (void *)&device_settings.mode, 0, 2, get_menu_mode_string);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "SAVE", TYPE_SAVE, "", (void *)&ssave, 0, 16, memory_get_preset_name);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "LOAD SAVE", TYPE_SAVE, "", (void *)&lsave, 0, 16, memory_get_preset_name);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "DELETE SAVE", TYPE_SAVE, "", (void *)&dsave, 0, 16, memory_get_preset_name);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "FORMAT", TYPE_CONFIRM, "", (void *)&format, 0, 1, get_menu_confirm_string);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "FAC RESET", TYPE_CONFIRM, "", (void *)&fac_reset, 0, 1, get_menu_confirm_string);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0, NULL);
    menu_add_item(&settings_menu, tmp_item);

    for (uint16_t i = 0; i < (MAX_DMX_CHANNELS / 16); i++) {
        char menu_item_short_name[10];
        for (uint16_t j = 0; j < (MENU_MAX_ITEMS - 1); j++) {
            sprintf(menu_item_short_name, "DMX%d", i * (MAX_DMX_CHANNELS / 16) + j);
            menu_create_item(&tmp_item, menu_item_short_name, TYPE_DMX_CH, "", dmx_values + i * 16 + j, -1, 256, get_menu_dmx_ch_string);
            menu_add_item(p_to_dmx_group_menus[i], tmp_item);
        }
        menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&static_channels_menu, 0, 0, NULL);
        menu_add_item(p_to_dmx_group_menus[i], tmp_item);

        sprintf(menu_item_short_name, "DMX%d-%d", i * 16, i * 16 + 15);
        menu_create_item(&tmp_item, menu_item_short_name, TYPE_MENU, "", (void *)p_to_dmx_group_menus[i], 0, 0, NULL);
        menu_add_item(&static_channels_menu, tmp_item);
    }

    menu_create_item(&tmp_item, "STATIC_CH", TYPE_MENU, "", (void *)&static_channels_menu, 0, 0, NULL);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0, NULL);
    menu_add_item(&static_channels_menu, tmp_item);

    for (uint8_t i = 0; i < 5; i++) {
        menu_create_item(&tmp_item,  "ENABLE", TYPE_ENUM, "", (void *)&p_to_channels[i]->enabled, 0, 1, get_menu_enable_string);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item,  "INPUT", TYPE_UINT8, "", (void *)&p_to_channels[i]->input_channel, 1, 5, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "DMX CHAN", TYPE_UINT8, "", (void *)&p_to_channels[i]->dmx_ch, 0, 256, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "LEVEL", TYPE_UINT8, "", (void *)&p_to_channels[i]->level, 0, 31, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "ATTACK", TYPE_UINT32, "ms", (void *)&p_to_channels[i]->adsr->attack, 0, 5000, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "DECAY", TYPE_UINT32, "ms", (void *)&p_to_channels[i]->adsr->decay, 0, 5000, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item,  "SUSTAIN", TYPE_UINT32, "ms", (void *)&p_to_channels[i]->adsr->sustain, 0, 5000, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item,  "S_LEVEL", TYPE_UINT8, "", (void *)&p_to_channels[i]->adsr->sustain_level, 0, 255, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "RELEASE", TYPE_UINT32, "ms", (void *)&p_to_channels[i]->adsr->release, 0, 5000, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item,  "MIDI CH", TYPE_UINT8, "", (void *)&p_to_channels[i]->midi_ch, 1, 16, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item,  "MIDI NOTE", TYPE_UINT8, "", (void *)&p_to_channels[i]->note, 0, 127, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0, NULL);
        menu_add_item(p_to_menus[i], tmp_item);
    }

    uint8_t prev_contrast = 0;
    uint8_t prev_brightness = 0;

    uint32_t send_data_timer = 0, read_button_timer = 0;
    uint8_t prev_compare_vals[5] = {0, 0, 0, 0, 0};
    menu_draw();
    uint32_t encoder_timer = 0;
    while (1) {
        if(state == SCROLL) {
            if(format == 1) {
                format = 0;
                memory_full_format();
            }
            if(lsave) {
                memory_load_preset(p_to_channels, &device_settings, dmx_values, lsave);
                lsave = 0;
            }
            if(ssave) {
                memory_write_preset(p_to_channels, &device_settings, dmx_values, ssave);
                ssave = 0;
            }
            if(dsave) {
                memory_delete_save(dsave);
                dsave=0;
            }
            if(fac_reset) {
                device_factory_reset();
                fac_reset = 0;
            }
        }
        if(key_pressed)
            for(uint8_t i = 0; i < 3; i++) {
                if(p_to_channels[i]->level != prev_compare_vals[i]) {
                    prev_compare_vals[i] = p_to_channels[i]->level;
                    switch(i) {
                        case 0:
                            ac_set_scaler(2, p_to_channels[i]->level);
                            break;
                        case 1:
                            ac_set_scaler(0, p_to_channels[i]->level);
                            break;
                        case 2:
                            ac_set_scaler(1, p_to_channels[i]->level);
                            break;
                    }
                }
            }

        if (device_settings.contrast != prev_contrast) {
            prev_contrast = device_settings.contrast;
            dac_chan_write(&dac_instance, DAC_CHANNEL_0, (device_settings.contrast << 5));
        }

        if (device_settings.brightness != prev_brightness) {
            set_brightness(device_settings.brightness);
            prev_brightness = device_settings.brightness;
        }

        if (key_pressed) {
            menu_draw();
        }

        key_pressed = 1;
        if(millis() - encoder_timer > 1) {
            encoder_timer = millis();
            switch (get_encoder_status()) {
                case BACKWARD:
                    for (uint8_t i = get_encoder_speed(); i > 0; i--)
                        if (state == SCROLL) {
                            decrement_menu_position(selected_menu);
                        }
                        else
                            menu_decrement_item(selected_menu);
                    break;
                case FORWARD:
                    for (uint8_t i = get_encoder_speed(); i > 0; i--)
                        if (state == SCROLL) {
                            increment_menu_position(selected_menu);
                        }
                        else
                            menu_increment_item(selected_menu);
                    break;
                default:
                    key_pressed = 0;
                    break;
            }
        }


        if (millis() - read_button_timer > 2) {
            read_button_timer = millis();
            key_pressed = button_handler(button_read(), &state);
        }

        if (millis() - send_data_timer > 40) {
            send_data_timer = millis();
            device_mode = select_device_mode(device_settings.mode);

            for(uint8_t i = 0; i < 5; i++)
                if(p_to_channels[i]->enabled)
                    dmx_values[p_to_channels[i]->dmx_ch] = adsr_get_value(adsr_channels + i);

            if (device_mode == DMX || device_mode == BOTH) {
                DMX_SendMessage(dmx_values, sizeof(dmx_values));
            }
            if (device_mode == TRIGGER || device_mode == BOTH) {
                for(uint8_t i = 0; i < 5; i++)
                    if((get_trigger_flags() >> i) & 0x1) {
                        MIDI_send_note_on(p_to_channels[i]->midi_ch, p_to_channels[i]->note,255);
                        MIDI_send_note_off(p_to_channels[i]->midi_ch, p_to_channels[i]->note,255);
                    }
                clear_trigger_flags();
            }
        }
    }
}

MODE select_device_mode(uint8_t mode)
{
    switch (mode) {
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

void IO_init(void)
{
    //configure all ports!
    struct port_config input_pin_no_pullup;
    port_get_config_defaults(&input_pin_no_pullup);
    input_pin_no_pullup.direction = PORT_PIN_DIR_INPUT;
    input_pin_no_pullup.input_pull = PORT_PIN_PULL_NONE;

    struct port_config input_pin_pullup;
    port_get_config_defaults(&input_pin_pullup);
    input_pin_pullup.direction = PORT_PIN_DIR_INPUT;
    input_pin_pullup.input_pull = PORT_PIN_PULL_UP;

    struct port_config output_pin;
    port_get_config_defaults(&output_pin);
    output_pin.direction = PORT_PIN_DIR_OUTPUT;
    struct port_config output_pin_w_readback;
    port_get_config_defaults(&output_pin_w_readback);
    output_pin_w_readback.direction = PORT_PIN_DIR_OUTPUT_WTH_READBACK;
    /* DEFINE PINS WITH PULLUPS */
    port_pin_set_config(PIN_SW1, &input_pin_pullup);
    port_pin_set_config(PIN_SW2, &input_pin_no_pullup);

    /* DEFINE PINS WITHOUT PULLUPS */
    port_pin_set_config(PIN_ENC_B, &input_pin_no_pullup);
    port_pin_set_config(PIN_ENC_A, &input_pin_no_pullup);

    /* DEFINE OUTPUT PINS */
    port_pin_set_config(PIN_EEPROM_WP, &output_pin);
    port_pin_set_config(PIN_LED_POWER, &output_pin);
    port_pin_set_config(PIN_LCD_D0, &output_pin);
    port_pin_set_config(PIN_LCD_D1, &output_pin);
    port_pin_set_config(PIN_LCD_D2, &output_pin);
    port_pin_set_config(PIN_LCD_D3, &output_pin);
    port_pin_set_config(PIN_LCD_D4, &output_pin);
    port_pin_set_config(PIN_LCD_D5, &output_pin);
    port_pin_set_config(PIN_LCD_D6, &output_pin);
    port_pin_set_config(PIN_LCD_D7, &output_pin);
    port_pin_set_config(PIN_LCD_EN, &output_pin);
    port_pin_set_config(PIN_LCD_RW, &output_pin);
    port_pin_set_config(PIN_LCD_RS, &output_pin);
    port_pin_set_config(PIN_DMX_DIR, &output_pin);
    port_pin_set_output_level(PIN_EEPROM_WP, 0);
    struct system_pinmux_config mux_config;
    system_pinmux_get_config_defaults(&mux_config);
    mux_config.mux_position = MUX_ADC0;
    system_pinmux_pin_set_config(PIN_ADC0, &mux_config);

    mux_config.mux_position = MUX_ADC1;
    system_pinmux_pin_set_config(PIN_ADC1, &mux_config);
    mux_config.mux_position = MUX_ADC2;
    system_pinmux_pin_set_config(PIN_ADC2, &mux_config);
    mux_config.mux_position = MUX_ADC3;
    system_pinmux_pin_set_config(PIN_ADC3, &mux_config);
    mux_config.mux_position = MUX_ADC4;
    system_pinmux_pin_set_config(PIN_ADC4, &mux_config);

    mux_config.mux_position = MUX_LCD_VO;
    system_pinmux_pin_set_config(PIN_LCD_VO, &mux_config);
}

void configure_dac(void)
{
    struct dac_config config_dac;
    dac_get_config_defaults(&config_dac);
    config_dac.reference = DAC_REFERENCE_AVCC;
    dac_init(&dac_instance, DAC, &config_dac);
}

void configure_dac_channel(void)
{
    struct dac_chan_config config_dac_chan;
    dac_chan_get_config_defaults(&config_dac_chan);
    dac_chan_set_config(&dac_instance, DAC_CHANNEL_0, &config_dac_chan);
    dac_chan_enable(&dac_instance, DAC_CHANNEL_0);
}

uint8_t button_handler(TIPKA t, STATE *s)
{

    switch (t) {
        /*case BUTTON_1:
            return 1;*/
        case BUTTON_2:
            if (get_p_to_item(selected_menu)->type == TYPE_MENU) {
                menu_swap(&selected_menu, (MENU *)(get_p_to_item(selected_menu)->variable));
                *s = SCROLL;
            }
            else if (*s == EDIT)
                *s = SCROLL;
            else if (*s == SCROLL)
                *s = EDIT;
            return 2;
        default:
            return 0;
            break;
    }
    return 0;
}

void menu_draw(void)
{
    char menu_string_array[4][21];

    menu_whole_string(selected_menu, menu_string_array[0], state);

    for (uint8_t i = 0; i < 4; i++) {
        lcd_setCursor(0, i);
        for(uint8_t j = 0; j<20; j++) {
            lcd_write(menu_string_array[i][j]);
        }
    }
}

void get_menu_mode_string(char *dest, uint8_t mode)
{
    switch(mode) {
        case 0:
            strcpy(dest, "DMX");
            break;
        case 1:
            strcpy(dest, "MIDI");
            break;
        case 2:
            strcpy(dest, "BOTH");
            break;
        default:
            strcpy(dest, "ERR");
            break;
    }
}

void get_menu_bar_string(char *dest, uint8_t value)
{
    char tmp_string[8];
    tmp_string[7] = 0;
    uint8_t col_size = 7;
    uint32_t tmp_value = value * col_size * 5 / 16;
    for(uint8_t i = 0; i < col_size; i++) {
        if(tmp_value >= ((i + 1) * 5))
            * (tmp_string + i) = 5;
        else if((tmp_value > (i * 5)) && (tmp_value <= (i * 5 + 5)) && (value != 0))
            * (tmp_string + i) = tmp_value % 5;
        else
            * (tmp_string + i) = ' ';
    }
    sprintf(dest, "%s", tmp_string);
}

void get_menu_enable_string(char *dest, uint8_t mode)
{
    switch(mode) {
        case 0:
            strcpy(dest, "OFF");
            break;
        case 1:
            strcpy(dest, "ON");
            break;
    }
}

void get_menu_dmx_ch_string(char *dest, uint8_t num)
{
    uint8_t i;
    for(i = 0; i < 5; i++) {
        if(p_to_channels[i]->dmx_ch == num && p_to_channels[i]->enabled)break;
        else {
            sprintf(dest, "%d", dmx_values[num]);
        }
    }
    if(i == 5)
        return;
    char input_string[] = "IN ";
    input_string[2] = '0' + p_to_channels[i]->input_channel;
    strcpy(dest, input_string);
}
void get_menu_confirm_string(char *dest, uint8_t num)
{
    if(num)
        strcpy(dest, "YES");
    else if(state == SCROLL)
        strcpy(dest, "");
    else
        strcpy(dest, "NO");

}
void device_factory_reset(void)
{
    device_settings.mode = 2;
    for (uint16_t i = 0; i < sizeof(dmx_values); i++) {
        dmx_values[i] = 0;
    }
    for(uint8_t i = 0; i < 5; i++) {
        adsr_init(adsr_channels + i, 1);
        adsr_channels[i].attack = 100;
        adsr_channels[i].decay = 100;
        adsr_channels[i].sustain = 100;
        adsr_channels[i].release = 100;
        adsr_channels[i].sustain_level = 127;
        trigger_channels[i].adsr = adsr_channels + i;
        p_to_channels[i] = trigger_channels + i;
        p_to_channels[i]->dmx_ch = i*8+6;
        p_to_channels[i]->note = MIDI_NOTE_A4;
        p_to_channels[i]->midi_ch = i + 1;
        p_to_channels[i]->input_channel = i + 1;
        p_to_channels[i]->enabled = 0;
        p_to_channels[i]->level = 16;
    }
}
