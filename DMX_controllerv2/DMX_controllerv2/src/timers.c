#include <asf.h>
#include <stdint.h>

#include "config.h"
#include "adsr.h"
#include "timers.h"
struct tcc_module tcc0_instance;
ADSR *a0, *a1, *a2, *a3, *a4;

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

}