
#ifndef __ADC_H_
#define __ADC_H_

void configure_adc0(ADSR *adsr);
void configure_adc0_callbacks(void);
void adc0_read_callback(void);
void adc0_window_callback(void);
void adc0_set_compare_value(uint8_t value);
void adc0_change_channel(uint8_t channel);

#endif /* ADC_H_ */