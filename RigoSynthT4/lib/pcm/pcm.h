#ifndef __PCM_H
#define __PCM_H

#include <Arduino.h>
#include <arm_math.h> // for float32_t type

// macros

#define INTPIN 2    // little green LED with a 100Ω resistor 

#define BCLK    21
#define WCLK    20
#define DOUT    7

// types

typedef enum {
    SYNTH_DDS         = 0,
    SYNTH_PHYS_STRING = 1,
} synth_mode_t;

// functions

uint32_t return_int_time(void);

void pcm_setup(void);

void set_volume(float32_t vol);

#endif /* __PCM_H */