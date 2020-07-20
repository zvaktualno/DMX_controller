#include <asf.h>
#include <stdint.h>

#include "config.h"
#include "adsr.h"
#include "timers.h"
struct tcc_module tcc0_instance;
ADSR *a0, *a1, *a2, *a3, *a4;
uint32_t proc_millis = 0;
ENCODER encoder;
ENCODER_POS encoder_pos;
uint32_t millis(void) {
    return proc_millis;
}

ENCODER get_encoder_status(void) {
    return encoder;
    encoder = IDLE;
}

void process_encoder(void) {
    uint8_t status = 0;
    status |= !port_pin_get_input_level(PIN_ENC_A) << 0;
    status |= !port_pin_get_input_level(PIN_ENC_B) << 1;
    switch(status) {
        case 0:
            encoder_pos = POS_NONE;
            break;
        case 1:
            if(encoder_pos == POS_NONE) {
                encoder_pos = POS_A;
                encoder = BACKWARD;
            }
            if(encoder_pos == POS_B)
                encoder = IDLE;
            break;
        case 2:
            if(encoder_pos == POS_NONE) {
                encoder_pos = POS_B;
                encoder = FORWARD;
            }
            if(encoder_pos == POS_A)
                encoder = IDLE;
            break;
        default:
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

void configure_tcc0_callbacks(ADSR *adsr0, ADSR *adsr1, ADSR *adsr2, ADSR *adsr3, ADSR *adsr4) {
    process_encoder();
    tcc_register_callback(&tcc0_instance, timer0_compare_callback, TCC_CALLBACK_OVERFLOW);
    tcc_enable_callback(&tcc0_instance, TCC_CALLBACK_OVERFLOW);
    a0 = adsr0;
    a1 = adsr1;
    a2 = adsr2;
    a3 = adsr3;
    a4 = adsr4;
}

void timer0_compare_callback(struct tcc_module *const module_inst) {
    process_ADSR(a0);
    process_ADSR(a1);
    process_ADSR(a2);
    process_ADSR(a3);
    process_ADSR(a4);
    proc_millis++;
}