#include "random.h"

#include <Entropy.h>

void random_setup(void){
    Entropy.Initialize();
}

uint32_t random_gen(uint32_t min, uint32_t max){
    Entropy.random(min, max);
}