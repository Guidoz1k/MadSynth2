#ifndef __RANDOM_H
#define __RANDOM_H

#include <Arduino.h>

void random_setup(void);

uint32_t random_gen(uint32_t min, uint32_t max);

#endif /* __RANDOM_H */