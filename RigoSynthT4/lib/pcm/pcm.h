#ifndef __PCM_H
#define __PCM_H

#include <Arduino.h>

#define INTPIN 2    // little green LED with a 100Ω resistor 

#define BCLK    21
#define WCLK    20
#define DOUT    7

uint32_t return_dds_time(void);

void i2s_setup(void);

void timerInterrupt();

void set_frequency(uint16_t freq);

void set_volume(float vol);

#endif /* __PCM_H */