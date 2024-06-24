#ifndef __PCM_H
#define __PCM_H

#include <Arduino.h>

#define INTPIN 24

#define BCLK    21
#define WCLK    20
#define DOUT    7

void i2s_setup(void);

void timerInterrupt();

void frequency(uint16_t freq);

void set_volume(float vol);

#endif /* __PCM_H */