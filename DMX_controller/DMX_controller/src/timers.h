
#ifndef TIMERS_H_
#define TIMERS_H_

typedef enum {
    IDLE,
    FORWARD,
    BACKWARD,
} ENCODER;
typedef enum {
    POS_NONE,
    POS_A,
    POS_B,
} ENCODER_POS;
void configure_tcc0(void);
void configure_tcc0_callbacks(ADSR *adsr);
void timer0_compare_callback(struct tcc_module *const module_inst);
uint32_t millis(void);
uint8_t get_encoder_speed(void);
ENCODER get_encoder_status(void);
void process_encoder(void);
void configure_tc0(void);
void set_brightness(uint8_t brightness);
#endif /* TIMERS_H_ */