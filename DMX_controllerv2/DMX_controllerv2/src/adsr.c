
#include <stdint.h>
#include "adsr.h"
#include "config.h"

#define MAX_32BIT_VALUE 0xFFFFFFFF

void process_ADSR(ADSR *adsr) {
    switch(adsr->state) {
        case START:
            adsr->attack_counter = 0;
            adsr->sustain_counter = 0;
            adsr->decay_counter = 0;
            adsr->release_counter = 0;
            //adsr->value = 0;
            adsr->state = ATTACK;
            break;

        case ATTACK:
            adsr->attack_counter += adsr->interval;
            //adsr->value = 0xFFFFFFFF;
            adsr->value = MAX_32BIT_VALUE / adsr->attack  * adsr->attack_counter;
            if(adsr->attack_counter > adsr->attack) {
                adsr->value = MAX_32BIT_VALUE; //max value
                adsr->state = DECAY;
                adsr->attack_counter = 0;
            }
            break;
        case DECAY:
            adsr->decay_counter += adsr->interval;
            adsr->value =  MAX_32BIT_VALUE - (MAX_32BIT_VALUE - adsr->sustain_level) / adsr->decay * adsr->decay_counter ;
            if(adsr->decay_counter > adsr->decay) {
                adsr->value = adsr->sustain_level;
                adsr->state = SUSTAIN;
                adsr->decay_counter = 0;
            }
            break;
        case SUSTAIN:
            adsr->sustain_counter += adsr->interval;
            adsr->value = adsr->sustain_level;
            if(adsr->sustain_counter > adsr->sustain) {
                adsr->state = RELEASE;
                adsr->sustain_counter = 0;
            }
            break;
        case RELEASE:
            adsr->release_counter += adsr->interval;
            adsr->value = adsr->sustain_level - adsr->sustain_level / adsr->release * adsr->release_counter ;
            if(adsr->release_counter > adsr->release) {
                adsr->value = 0;
                adsr->state = END;
                adsr->release_counter = 0;
            }
            break;
        case END:
            adsr->value = 0;
            break;
    }
}