
#include <stdint.h>
#include "adsr.h"
#include "config.h"

#define MAX_32BIT_VALUE 0xFFFFFFFF

void adsr_process(ADSR *adsr)
{
    switch(adsr->state) {
        case START:
            adsr->attack_counter = 0;
            adsr->sustain_counter = 0;
            adsr->decay_counter = 0;
            adsr->release_counter = 0;
            adsr->state = ATTACK;
            adsr->value = 0;
            break;

        case ATTACK:
            if(adsr->attack==0) {
                adsr->value = MAX_32BIT_VALUE;
                adsr->state = DECAY;
                break;
            }
            adsr->value = MAX_32BIT_VALUE / adsr->attack  * adsr->attack_counter;
            adsr->attack_counter += adsr->interval;

            if(adsr->attack_counter > adsr->attack) {
                adsr->value = MAX_32BIT_VALUE; //max value
                adsr->state = DECAY;
                adsr->attack_counter = 0;
            }
            break;
        case DECAY:
            if(adsr->decay==0) {
                adsr->value = adsr->sustain_level;

                adsr->state = SUSTAIN;
                break;
            }

            adsr->value =  MAX_32BIT_VALUE - (MAX_32BIT_VALUE - (adsr->sustain_level<<24)) / adsr->decay * adsr->decay_counter ;
            adsr->decay_counter += adsr->interval;
            if(adsr->decay_counter > adsr->decay) {
                adsr->value = adsr->sustain_level<<24;
                adsr->state = SUSTAIN;
                adsr->decay_counter = 0;
            }
            break;
        case SUSTAIN:
            if(adsr->sustain==0) {
                adsr->state = RELEASE;
                break;
            }

            adsr->value = adsr->sustain_level<<24;
            adsr->sustain_counter += adsr->interval;
            if(adsr->sustain_counter > adsr->sustain) {
                adsr->state = RELEASE;
                adsr->sustain_counter = 0;
            }
            break;
        case RELEASE:
            if(adsr->release==0) {
                adsr->state = END;
                break;
            }
            adsr->value = ((adsr->sustain_level<<24) - (adsr->sustain_level<<24) / adsr->release * adsr->release_counter);
            adsr->release_counter += adsr->interval;
            if(adsr->release_counter > adsr->release) {
                adsr->value = 0;
                adsr->state = END;
                adsr->release_counter = 0;
            }
            break;
        case END:
            adsr->value=0;
            break;
    }
}
uint8_t adsr_get_value(ADSR *adsr)
{
    return adsr->value >> 24;
}

void adsr_trigger(ADSR *adsr)
{
    adsr->state=START;
}

void adsr_init(ADSR *adsr, uint32_t interval)
{
    adsr->attack_counter = 0;
    adsr->decay_counter = 0;
    adsr->sustain_counter = 0;
    adsr->release_counter = 0;
    adsr->value = 0;
    adsr->state = END;
    adsr->interval=interval;
    adsr->attack=100;
    adsr->decay=100;
    adsr->sustain=100;
    adsr->release=100;
    adsr->sustain_level=128;




}