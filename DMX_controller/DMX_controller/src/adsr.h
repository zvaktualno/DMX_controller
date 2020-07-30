/*
 * adsr.h
 *
 * Created: 22. 05. 2020 13:26:56
 *  Author: Uporabnik
 */


#ifndef _ADSR_H_
#define _ADSR_H_


typedef enum {
    START,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    END
} ADSR_STATE;

typedef struct {
    uint32_t value;				//32-bit output value
    uint32_t interval;			//process()'s period of calling
    uint32_t attack;			//time in millis
    uint32_t attack_counter;	//helper counter
    uint32_t decay;				//time in millis
    uint32_t decay_counter;		//helper counter
    uint32_t sustain;			//time in millis
    uint32_t sustain_level;		//32 bit sustain level, should be set to xV/5V*0xFFFFFFFF
    uint32_t sustain_counter;	//helper counter
    uint32_t release;			//time in millis
    uint32_t release_counter;	//helper counter
    ADSR_STATE state;			//state of the ADSR(A, D, S, R)
    uint8_t debounce_timer;
} ADSR;


void adsr_process(ADSR *adsr);
uint8_t adsr_get_value(ADSR *adsr);
void adsr_trigger(ADSR *adsr);
void adsr_init(ADSR *adsr, uint32_t interval);
uint8_t get_trigger_flags(void);
void clear_trigger_flags(void);
#endif /* ADSR_H_ */