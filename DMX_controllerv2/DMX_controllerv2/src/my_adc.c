#include <asf.h>
#include "config.h"
#include "my_adc.h"
#include "adsr.h"



volatile uint8_t adc0_read_done_flag;
ADSR *adsr0, *adsr1, *adsr2, *adsr3, *adsr4;
uint8_t adc_ch = 0;
int16_t result[5];

struct adc_module adc0_instance;
static struct ac_module ac_instance;

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
    config_adc.positive_input                = ADC_POSITIVE_INPUT_PIN6;
    config_adc.negative_input                = ADC_NEGATIVE_INPUT_GND;
    config_adc.left_adjust                   = false;
    config_adc.freerunning                   = true;
    config_adc.positive_input_sequence_mask_enable = 0;

    adc_init(&adc0_instance, ADC0, &config_adc);
    adc_enable(&adc0_instance);
    adc0_change_channel(adc_ch);
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

    if(++adc_ch == 2)adc_ch = 0;
    adc0_change_channel(adc_ch);
    adc_read_buffer_job(&adc0_instance, result + adc_ch, 1);

}
void adc0_window_callback(void) {
    switch(adc_ch) {
        case 0:
            adsr_trigger(adsr3);
            break;
        case 1:
            adsr_trigger(adsr4);
            break;
    }
}
void adc0_set_compare_value(uint8_t value) {
    adc_set_window_mode(&adc0_instance, ADC_WINDOW_MODE_ABOVE_LOWER, 90, 255);
}
void adc0_change_channel(uint8_t channel) {
    if(channel > 1)
        return;
    const enum adc_positive_input adc_pin[5] = {ADC_POSITIVE_INPUT_PIN6,
                                                ADC_POSITIVE_INPUT_PIN7,
                                               };
    adc_set_positive_input(&adc0_instance, adc_pin[channel]);
}




void configure_ac(ADSR *adsr) {

    adsr0 = adsr + 0;
    adsr1 = adsr + 1;
    adsr2 = adsr + 2;
    adsr3 = adsr + 3;
    adsr4 = adsr + 4;

    struct ac_config config_ac;
    ac_get_config_defaults(&config_ac);
    config_ac.source_generator = GCLK_GENERATOR_5;
    ac_init(&ac_instance, AC, &config_ac);

    configure_ac_channel0();
    configure_ac_channel1();
    configure_ac_channel2();
    configure_ac_callback();
    ac_enable(&ac_instance);

}

void configure_ac_channel0(void) {

    struct ac_chan_config config_ac_chan;

    ac_chan_get_config_defaults(&config_ac_chan);
    config_ac_chan.sample_mode         = AC_CHAN_MODE_CONTINUOUS;
    config_ac_chan.filter              = AC_CHAN_FILTER_NONE;
    config_ac_chan.enable_hysteresis   = false;
    config_ac_chan.output_mode         = AC_CHAN_OUTPUT_INTERNAL;
    config_ac_chan.positive_input      = AC_CHAN_POS_MUX_PIN1;
    config_ac_chan.negative_input      = AC_CHAN_NEG_MUX_SCALED_VCC;
    config_ac_chan.vcc_scale_factor    = 32;

    config_ac_chan.interrupt_selection = AC_CHAN_INTERRUPT_SELECTION_TOGGLE;

    ac_chan_set_config(&ac_instance, AC_CHAN_CHANNEL_2, &config_ac_chan);

    ac_chan_enable(&ac_instance, AC_CHAN_CHANNEL_2);
}
void configure_ac_channel1(void) {

    struct ac_chan_config config_ac_chan;

    ac_chan_get_config_defaults(&config_ac_chan);
    config_ac_chan.sample_mode         = AC_CHAN_MODE_CONTINUOUS;
    config_ac_chan.filter              = AC_CHAN_FILTER_NONE;
    config_ac_chan.enable_hysteresis   = false;
    config_ac_chan.output_mode         = AC_CHAN_OUTPUT_INTERNAL;
    config_ac_chan.positive_input      = AC_CHAN_POS_MUX_PIN0;
    config_ac_chan.negative_input      = AC_CHAN_NEG_MUX_SCALED_VCC;
    config_ac_chan.vcc_scale_factor    = 32;

    config_ac_chan.interrupt_selection = AC_CHAN_INTERRUPT_SELECTION_TOGGLE;

    ac_chan_set_config(&ac_instance, AC_CHAN_CHANNEL_0, &config_ac_chan);

    ac_chan_enable(&ac_instance, AC_CHAN_CHANNEL_0);
}
void configure_ac_channel2(void) {

    struct ac_chan_config config_ac_chan;

    ac_chan_get_config_defaults(&config_ac_chan);
    config_ac_chan.sample_mode         = AC_CHAN_MODE_CONTINUOUS;
    config_ac_chan.filter              = AC_CHAN_FILTER_NONE;
    config_ac_chan.enable_hysteresis   = false;
    config_ac_chan.output_mode         = AC_CHAN_OUTPUT_INTERNAL;
    config_ac_chan.positive_input      = AC_CHAN_POS_MUX_PIN1;
    config_ac_chan.negative_input      = AC_CHAN_NEG_MUX_SCALED_VCC;
    config_ac_chan.vcc_scale_factor    = 32;

    config_ac_chan.interrupt_selection = AC_CHAN_INTERRUPT_SELECTION_TOGGLE;

    ac_chan_set_config(&ac_instance, AC_CHAN_CHANNEL_1, &config_ac_chan);

    ac_chan_enable(&ac_instance, AC_CHAN_CHANNEL_1);
}


void callback_function_ac0(struct ac_module *const module_inst) {
    adsr_trigger(adsr0);
}
void callback_function_ac1(struct ac_module *const module_inst) {
    adsr_trigger(adsr1);
}
void callback_function_ac2(struct ac_module *const module_inst) {
    adsr_trigger(adsr2);
}

void configure_ac_callback(void) {
    ac_register_callback(&ac_instance, callback_function_ac0, AC_CALLBACK_COMPARATOR_2);
    ac_register_callback(&ac_instance, callback_function_ac1, AC_CALLBACK_COMPARATOR_0);
    ac_register_callback(&ac_instance, callback_function_ac2, AC_CALLBACK_COMPARATOR_1);
    ac_enable_callback(&ac_instance, AC_CALLBACK_COMPARATOR_0);
    ac_enable_callback(&ac_instance, AC_CALLBACK_COMPARATOR_1);
    ac_enable_callback(&ac_instance, AC_CALLBACK_COMPARATOR_2);
}