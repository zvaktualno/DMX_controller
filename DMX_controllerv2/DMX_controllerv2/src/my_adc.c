#include <asf.h>
#include "config.h"
#include "my_adc.h"


struct adc_module adc0_instance;

volatile uint8_t adc0_read_done_flag;

void configure_adc0(void) {

    struct adc_config config_adc;
    adc_get_config_defaults(&config_adc);

    config_adc.clock_source                  = GCLK_GENERATOR_6;
    config_adc.reference                     = ADC_REFERENCE_INTVCC2;
    config_adc.clock_prescaler               = ADC_CLOCK_PRESCALER_DIV8;
    config_adc.resolution                    = ADC0_RESOLUTION;
    config_adc.window.window_mode            = ADC_WINDOW_MODE_ABOVE_LOWER;
    config_adc.window.window_upper_value     = 150;
    config_adc.window.window_lower_value     = 900;
    config_adc.positive_input                = ADC_POSITIVE_INPUT_PIN7;
    config_adc.negative_input                = ADC_NEGATIVE_INPUT_GND;
    config_adc.left_adjust                   = false;
    config_adc.freerunning                   = true;
    config_adc.positive_input_sequence_mask_enable = 0;

    adc_init(&adc0_instance, ADC0, &config_adc);
    adc_enable(&adc0_instance);
}

void configure_adc0_callbacks(void) {
    adc_register_callback(&adc0_instance, adc0_window_callback, ADC_CALLBACK_WINDOW);
    adc_enable_callback(&adc0_instance, ADC_CALLBACK_WINDOW);
    adc_register_callback(&adc0_instance, adc0_read_callback, ADC_CALLBACK_READ_BUFFER);
    adc_enable_callback(&adc0_instance, ADC_CALLBACK_READ_BUFFER);
}
void adc0_read_callback(void) {
    adc0_read_done_flag = 1;
    //menjaj kanal
}
void adc0_window_callback(void) {
    BREAKPOINT;
    //sprozi ADSRje
}

void adc0_set_compare_value(uint8_t value) {
    adc_set_window_mode(&adc0_instance, ADC_WINDOW_MODE_BELOW_UPPER, 1100, 150);
}
void adc0_change_channel(uint8_t channel) {
    const enum adc_positive_input adc_pin[12] = {ADC_POSITIVE_INPUT_PIN7,
                                                 ADC_POSITIVE_INPUT_PIN6,
                                                 ADC_POSITIVE_INPUT_PIN5,
                                                 ADC_POSITIVE_INPUT_PIN4,
                                                 ADC_POSITIVE_INPUT_PIN1,
                                                };
    adc_set_positive_input(&adc0_instance, adc_pin[channel]);
}