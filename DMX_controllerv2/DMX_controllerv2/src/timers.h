
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
void configure_tcc0_callbacks(ADSR *adsr0, ADSR *adsr1, ADSR *adsr2, ADSR *adsr3, ADSR *adsr4);
void timer0_compare_callback(struct tcc_module *const module_inst);
uint32_t millis(void);

ENCODER get_encoder_status(void);
void process_encoder(void);
#endif /* TIMERS_H_ */