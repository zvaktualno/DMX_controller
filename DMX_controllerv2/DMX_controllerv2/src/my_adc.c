#include <asf.h>
#include "config.h"
#include "my_adc.h"
#include "adsr.h"

struct adc_module adc0_instance;

volatile uint8_t adc0_read_done_flag;
ADSR *adsr0, *adsr1, *adsr2, *adsr3, *adsr4;
uint8_t adc_ch = 0;
int16_t result[5];
void configure_adc0(ADSR *adsr) {
    adsr0 = adsr + 0;
    adsr1 = adsr + 1;
    adsr2 = adsr + 2;
    adsr3 = adsr + 3;
    adsr4 = adsr + 4;
    struct adc_config config_adc;
    adc_get_config_defaults(&config_adc);

    config_adc.clock_source                  = GCLK_GENERATOR_7;
    config_adc.reference                     = ADC_REFERENCE_INTVCC2;
    config_adc.clock_prescaler               = ADC_CLOCK_PRESCALER_DIV8;
    config_adc.resolution                    = ADC_RESOLUTION_10BIT;
    config_adc.window.window_mode            = ADC_WINDOW_MODE_ABOVE_LOWER;
    config_adc.window.window_upper_value     = 0;
    config_adc.window.window_lower_value     = 150;
    config_adc.positive_input                = ADC_POSITIVE_INPUT_PIN7;
    config_adc.negative_input                = ADC_NEGATIVE_INPUT_GND;
    config_adc.left_adjust                   = false;
    config_adc.freerunning                   = true;
    config_adc.positive_input_sequence_mask_enable = 0;

    adc_init(&adc0_instance, ADC0, &config_adc);
    adc_enable(&adc0_instance);
    configure_adc0_callbacks();
    adc_read_buffer_job(&adc0_instance, &result, 1);
}

void configure_adc0_callbacks(void) {
    adc_register_callback(&adc0_instance, adc0_window_callback, ADC_CALLBACK_WINDOW);
    adc_enable_callback(&adc0_instance, ADC_CALLBACK_WINDOW);
    adc_register_callback(&adc0_instance, adc0_read_callback, ADC_CALLBACK_READ_BUFFER);
    adc_enable_callback(&adc0_instance, ADC_CALLBACK_READ_BUFFER);
}
void adc0_read_callback(void) {

    adc0_read_done_flag = 1;
    /*  if(result[adc_ch]>150)
          switch(adc_ch) {
              case 0:
                  adsr_trigger(adsr0);
                  break;
              case 1:
                  adsr_trigger(adsr1);
                  break;
              case 2:
                  adsr_trigger(adsr2);
                  break;
              case 3:
                  adsr_trigger(adsr3);
                  break;
              case 4:
                  adsr_trigger(adsr4);
                  break;
          }*/
    if(++adc_ch == 5)adc_ch = 0;
    adc0_change_channel(adc_ch);
    adc_read_buffer_job(&adc0_instance, result + adc_ch, 1);

}
void adc0_window_callback(void) {
    switch(adc_ch - 2) {
        case 0:
            adsr_trigger(adsr0);
            break;
        case 1:
            adsr_trigger(adsr1);
            break;
        case 2:
            adsr_trigger(adsr2);
            break;
        case 3:
            adsr_trigger(adsr3);
            break;
        case 4:
            adsr_trigger(adsr4);
            break;
            //sprozi ADSRje
    }
}
void adc0_set_compare_value(uint8_t value) {
    adc_set_window_mode(&adc0_instance, ADC_WINDOW_MODE_ABOVE_LOWER, 90, 255);
}
void adc0_change_channel(uint8_t channel) {
    const enum adc_positive_input adc_pin[5] = {ADC_POSITIVE_INPUT_PIN7,
                                                ADC_POSITIVE_INPUT_PIN6,
                                                ADC_POSITIVE_INPUT_PIN5,
                                                ADC_POSITIVE_INPUT_PIN4,
                                                ADC_POSITIVE_INPUT_PIN1,
                                               };
    adc_set_positive_input(&adc0_instance, adc_pin[channel]);
}

