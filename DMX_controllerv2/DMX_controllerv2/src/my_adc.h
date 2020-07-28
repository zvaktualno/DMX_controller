
#ifndef __ADC_H_
#define __ADC_H_

void configure_adc0(ADSR *adsr);
void configure_adc0_callbacks(void);
void adc0_read_callback(void);
void adc0_window_callback(void);
void adc0_set_compare_value(uint8_t value);
void adc0_change_channel(uint8_t channel);
void configure_ac(ADSR *adsr);
void configure_ac_channel0(void);
void configure_ac_channel1(void);
void configure_ac_channel2(void);
void callback_function_ac0(struct ac_module *const module_inst);
void callback_function_ac1(struct ac_module *const module_inst);
void callback_function_ac2(struct ac_module *const module_inst);
void configure_ac_callback(void);

#endif /* ADC_H_ */