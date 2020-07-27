
#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "adsr.h"
#include "timers.h"
#include "my_adc.h"
#include "USB.h"
#include "DMX.h"
//#include "my_i2c.h"
#include "my_menu.h"
#include "2004LCD.h"
#include "tipke.h"

typedef enum
{
    TRIGGER,
    DMX,
    BOTH
} MODE;

MODE device_mode;
MODE select_device_mode(uint8_t mode);
void configure_dac(void);
void configure_dac_channel(void);
void button_handler(TIPKA t, STATE *s);
void menu_draw(void);

struct dac_module dac_instance;
void IO_init(void);

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

uint8_t dmx_values[256];
ADSR adsr_channel0;
ADSR adsr_channel1;
ADSR adsr_channel2;
ADSR adsr_channel3;
ADSR adsr_channel4;

channel trigger_channel0;
channel trigger_channel1;
channel trigger_channel2;
channel trigger_channel3;
channel trigger_channel4;

STATE state = SCROLL;
SETTINGS device_settings = {.contrast = 8};
MENU *selected_menu = &main_menu;
uint8_t key_pressed = 1;
int main(void)
{
    adsr_init(&adsr_channel0, 1);
    adsr_init(&adsr_channel1, 1);
    adsr_init(&adsr_channel2, 1);
    adsr_init(&adsr_channel3, 1);
    adsr_init(&adsr_channel4, 1);

    adsr_channel0.attack = 1000;
    adsr_channel0.decay = 1000;
    adsr_channel0.sustain = 1000;
    adsr_channel0.release = 1000;
    adsr_channel0.sustain_level = 127;

    trigger_channel0.adsr = &adsr_channel0;
    trigger_channel1.adsr = &adsr_channel1;
    trigger_channel2.adsr = &adsr_channel2;
    trigger_channel3.adsr = &adsr_channel3;
    trigger_channel4.adsr = &adsr_channel4;
    system_init();
    delay_init();
    IO_init();
    lcd_begin();
    delay_ms(199);
    lcd_noCursor();
    configure_dac();
    configure_dac_channel();
    configure_USB();
    configure_DMX();
    configure_tc0();
    dac_enable(&dac_instance);

    for (uint16_t i = 0; i < sizeof(dmx_values); i++)
    {
        dmx_values[i] = 0;
    }

    Enable_global_interrupt();
    NVIC_SetPriority(ADC0_IRQn, 1);
    NVIC_SetPriority(DMX_IRQn, 2);

    configure_adc0(&adsr_channel0, &adsr_channel1, &adsr_channel2, &adsr_channel3, &adsr_channel4);
    adc0_set_compare_value(100);
    configure_tcc0();
    configure_tcc0_callbacks(&adsr_channel0, &adsr_channel1, &adsr_channel2, &adsr_channel3, &adsr_channel4);

    uint8_t device_mode_num = 0;
    MENU *p_to_dmx_group_menus[16];
    for (uint8_t i = 0; i < 16; i++)
    {
        p_to_dmx_group_menus[i] = (MENU *)malloc(sizeof(MENU));
    }
    channel *p_to_channels[5] = {&trigger_channel0, &trigger_channel1, &trigger_channel2, &trigger_channel3, &trigger_channel4};
    volatile MENU *p_to_menus[5] = {&channel1_menu, &channel2_menu, &channel3_menu, &channel4_menu, &channel5_menu};

    uint8_t brightness = 0, contrast = 0;
    menu_item tmp_item;

    menu_create_item(&tmp_item, "SETTINGS", TYPE_MENU, "", (void *)&settings_menu, 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 1", TYPE_MENU, "", (void *)p_to_menus[0], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 2", TYPE_MENU, "", (void *)p_to_menus[1], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 3", TYPE_MENU, "", (void *)p_to_menus[2], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 4", TYPE_MENU, "", (void *)p_to_menus[3], 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "CHANNEL 5", TYPE_MENU, "", (void *)p_to_menus[4], 0, 0);
    menu_add_item(&main_menu, tmp_item);

    menu_create_item(&tmp_item, "CONTRAST", UINT8, "", (void *)&device_settings.contrast, 0, 16);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "BRIGHTNES", UINT8, "", (void *)&device_settings.brightness, 0, 16);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "MODE", UINT8, "", (void *)&device_settings.mode, 0, 2);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "FIX_SIZE", UINT8, "", (void *)&device_settings.fixture_size, 7, 8);
    menu_add_item(&settings_menu, tmp_item);
    menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0);
    menu_add_item(&settings_menu, tmp_item);

    for (uint16_t i = 0; i < (MAX_DMX_CHANNELS / 16); i++)
    {
        char menu_item_name[10];

        for (uint16_t j = 0; j < MENU_MAX_ITEMS - 1; j++)
        {
            sprintf(menu_item_name, "DMX%d", i * MENU_MAX_ITEMS + j);
            menu_create_item(&tmp_item, menu_item_name, TYPE_UINT8, "", dmx_values + i * 16 + j, -1, 256);
            menu_add_item(p_to_dmx_group_menus[i], tmp_item);
        }
        menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&static_channels_menu, 0, 0);
        menu_add_item(p_to_dmx_group_menus[i], tmp_item);

        sprintf(menu_item_name, "DMX%d-%d", i * 16, i * 16 + 15);
        menu_create_item(&tmp_item, menu_item_name, TYPE_MENU, "", (void *)p_to_dmx_group_menus[i], 0, 0);
        menu_add_item(&static_channels_menu, tmp_item);
    }

    menu_create_item(&tmp_item, "STATIC_CH", TYPE_MENU, "", (void *)&static_channels_menu, 0, 0);
    menu_add_item(&main_menu, tmp_item);
    menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0);
    menu_add_item(&static_channels_menu, tmp_item);

    for (uint8_t i = 0; i < 5; i++)
    {
        menu_create_item(&tmp_item, "DMX CH", UINT8, "", (void *)&p_to_channels[i]->ch, 0, 255);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "LEVEL", FLOAT, "V", (void *)&p_to_channels[i]->level, 0, 5);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "ATTACK", UINT32, "ms", (void *)&p_to_channels[i]->adsr->attack, 0, 5000);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "DECAY", UINT32, "ms", (void *)&p_to_channels[i]->adsr->decay, 0, 5000);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "SUSTAIN", UINT32, "ms", (void *)&p_to_channels[i]->adsr->sustain, 0, 5000);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "S_LEVEL", UINT8, "", (void *)&p_to_channels[i]->adsr->sustain_level, 0, 255);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "RELEASE", UINT32, "ms", (void *)&p_to_channels[i]->adsr->release, 0, 5000);
        menu_add_item(p_to_menus[i], tmp_item);
        menu_create_item(&tmp_item, "BACK", TYPE_MENU, "", (void *)&main_menu, 0, 0);
        menu_add_item(p_to_menus[i], tmp_item);
    }

    uint8_t prev_contrast = 0;
    uint8_t prev_brightness = 0;

    for (uint16_t i = 0; i < sizeof(dmx_values); i++)
    {
        dmx_values[i] = 0;
    }

    dmx_values[0] = 255;
    dmx_values[6] = 255;

    uint32_t send_data_timer = 0, read_button_timer = 0;

    contrast = 9;
    while (1)
    {
        if (device_settings.contrast != prev_contrast)
        {
            prev_contrast = device_settings.contrast;
            dac_chan_write(&dac_instance, DAC_CHANNEL_0, (device_settings.contrast << 5));
        }

        if (device_settings.brightness != prev_brightness)
        {
            set_brightness(device_settings.brightness);
            prev_brightness = device_settings.brightness;
        }

        if (key_pressed)
        {
            menu_draw();
        }

        key_pressed = 1;
        switch (get_encoder_status())
        {
        case BACKWARD:
            for (uint8_t i = get_encoder_speed(); i > 0; i--)
                if (state == SCROLL)
                {
                    decrement_menu_position(selected_menu);
                }
                else
                    menu_decrement_item(selected_menu);
            break;
        case FORWARD:
            for (uint8_t i = get_encoder_speed(); i > 0; i--)
                if (state == SCROLL)
                {
                    increment_menu_position(selected_menu);
                }
                else
                    menu_increment_item(selected_menu);
            break;
        default:
            key_pressed = 0;
            break;
        }

        if (millis() - read_button_timer > 2)
        {
            read_button_timer = millis();
            button_handler(button_read(), &state);
        }

        if (port_pin_get_input_level(PIN_SW1) == 0)
            adsr_trigger(&adsr_channel0);

        if (millis() - send_data_timer > 70)
        {
            dmx_values[p_to_channels[0]->ch] = adsr_get_value(&adsr_channel0);
            /*dmx_values[1]=get_ADSR_value(&adsr_channel1);
            dmx_values[2]=get_ADSR_value(&adsr_channel2);
            dmx_values[3]=get_ADSR_value(&adsr_channel3);
            dmx_values[4]=get_ADSR_value(&adsr_channel4);*/

            send_data_timer = millis();
            device_mode = select_device_mode(device_mode_num);

            if (device_mode == DMX || device_mode == BOTH)
            {
                DMX_SendMessage(dmx_values, sizeof(dmx_values));
            }

            if (device_mode == TRIGGER || device_mode == BOTH)
            {
                BREAKPOINT;
                uint8_t usb_values[] = {adsr_get_value(&adsr_channel0), adsr_get_value(&adsr_channel1), adsr_get_value(&adsr_channel2), adsr_get_value(&adsr_channel3), adsr_get_value(&adsr_channel4)};
                USB_SendMessage(usb_values, 5);
            }
        }
    }
}

MODE select_device_mode(uint8_t mode)
{
    switch (mode)
    {
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

void button_handler(TIPKA t, STATE *s)
{
    key_pressed = 1;
    switch (t)
    {
    case BUTTON_1:
        break;
    case BUTTON_2:
        if (get_p_to_item(selected_menu)->type == TYPE_MENU)
        {
            menu_swap(&selected_menu, (MENU *)(get_p_to_item(selected_menu)->variable));
            *s = SCROLL;
        }
        else if (*s == EDIT)
            *s = SCROLL;
        else if (*s == SCROLL)
            *s = EDIT;
        break;
    default:
        key_pressed = 0;
        break;
    }
    return;
}
void menu_draw(void)
{
    char menu_string_array[4][21];
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 20; j++)
            menu_string_array[i][j] = ' ';
        menu_string_array[i][20] = 0;
    }
    menu_whole_string(selected_menu, menu_string_array, state);
    for (uint8_t i = 0; i < 4; i++)
    {
        lcd_setCursor(0, i);
        lcd_printstr(menu_string_array[i]);
    }
}