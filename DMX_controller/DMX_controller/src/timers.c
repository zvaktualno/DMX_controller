#include <asf.h>
#include <stdint.h>

#include "config.h"
#include "adsr.h"
#include "timers.h"
struct tcc_module tcc0_instance;
ADSR *a0, *a1, *a2, *a3, *a4;

struct tc_module tc1_instance;

volatile uint32_t proc_millis = 0;
volatile uint32_t encoder_timer = 0;
volatile uint32_t last_tick_time = 0;
ENCODER encoder;
ENCODER_POS encoder_pos;
uint32_t millis(void) {
    return proc_millis;
}

ENCODER get_encoder_status(void) {
    ENCODER tmp_status = encoder;
    encoder = IDLE;
    return tmp_status;

}

void process_encoder(void) {
    uint8_t status = 0;
    status |= (!port_pin_get_input_level(PIN_ENC_B)) << 0;
    status |= (!port_pin_get_input_level(PIN_ENC_A)) << 1;

    switch(status) {

        case 0:
            break;
        case 1:
            if(encoder_pos == POS_BOTH) {

                encoder = FORWARD;
                encoder_timer = millis() - last_tick_time;
                last_tick_time = millis();

            }
            encoder_pos = POS_A;

            break;
        case 2:
            if(encoder_pos == POS_BOTH) {

                encoder = BACKWARD;
                encoder_timer = millis() - last_tick_time;
                last_tick_time = millis();
            }
            encoder_pos = POS_B;

            break;
        case 3:
            encoder_pos = POS_BOTH;
            break;
    }
}

void configure_tcc0(void) {
    struct tcc_config config_tcc;
    tcc_get_config_defaults(&config_tcc, TCC0);

    config_tcc.counter.clock_source = GCLK_GENERATOR_6;
    config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV16;
    config_tcc.counter.period = 250;

    // change TCC module if needed, mind the pins for peripherals
    tcc_init(&tcc0_instance, TCC0, &config_tcc);
    tcc_enable(&tcc0_instance);
}

void configure_tcc0_callbacks(ADSR *adsr) {
    tcc_register_callback(&tcc0_instance, timer0_compare_callback, TCC_CALLBACK_OVERFLOW);
    tcc_enable_callback(&tcc0_instance, TCC_CALLBACK_OVERFLOW);
    a0 = adsr + 0;
    a1 = adsr + 1;
    a2 = adsr + 2;
    a3 = adsr + 3;
    a4 = adsr + 4;
}

void timer0_compare_callback(struct tcc_module *const module_inst) {
    adsr_process(a0);
    adsr_process(a1);
    adsr_process(a2);
    adsr_process(a3);
    adsr_process(a4);
    proc_millis++;
    process_encoder();
}
uint8_t get_encoder_speed(void) {
    if(encoder_timer > 30)return 1;
    if(encoder_timer > 20)return 3;
    if(encoder_timer > 10)return 5;
    return 30;
}

void configure_tc0(void) {
    struct tc_config config_tc;
    tc_get_config_defaults(&config_tc);

    config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
    config_tc.clock_source = GCLK_GENERATOR_7;
    config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
    config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
    config_tc.counter_8_bit.compare_capture_channel[1] = 0;
    config_tc.pwm_channel[1].enabled = true;
    config_tc.pwm_channel[1].pin_out = PIN_PB11E_TC1_WO1; //BUZZER_PIN;
    config_tc.pwm_channel[1].pin_mux = MUX_PB11E_TC1_WO1; //BUZZER_MUX;

    tc_init(&tc1_instance, TC1, &config_tc);
    tc_set_top_value(&tc1_instance, 64);
    tc_enable(&tc1_instance);
}

void set_brightness(uint8_t brightness) {
    brightness *= 4;
    tc_set_compare_value(&tc1_instance, TC_COMPARE_CAPTURE_CHANNEL_1, brightness);
}