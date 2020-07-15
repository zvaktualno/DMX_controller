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
} ADSR;

void process_ADSR(ADSR *adsr);

#endif /* ADSR_H_ */