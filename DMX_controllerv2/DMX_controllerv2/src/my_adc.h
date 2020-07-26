
#ifndef __ADC_H_
#define __ADC_H_

void configure_adc0(ADSR *a0, ADSR *a1, ADSR *a2, ADSR *a3, ADSR *a4);
void configure_adc0_callbacks(void);
void adc0_read_callback(void);
void adc0_window_callback(void);
void adc0_set_compare_value(uint8_t value);
void adc0_change_channel(uint8_t channel);

#endif /* ADC_H_ */