
#ifndef __ADC_H_
#define __ADC_H_

void configure_adc0(channel *p_to_ch);
void configure_adc0_callbacks(void);
void adc0_read_callback(void);
void adc0_window_callback(void);
void adc0_set_compare_value(uint16_t value);
void adc0_change_channel(uint8_t chan);
void configure_ac(channel *p_to_ch);
void configure_ac_channel0(void);
void configure_ac_channel1(void);
void configure_ac_channel2(void);
void callback_function_ac0(struct ac_module *const module_inst);
void callback_function_ac1(struct ac_module *const module_inst);
void callback_function_ac2(struct ac_module *const module_inst);
void configure_ac_callback(void);
void trigger_channel(uint8_t ch);
void ac_set_scaler(uint8_t ac_channel, uint8_t value);

#endif /* ADC_H_ */