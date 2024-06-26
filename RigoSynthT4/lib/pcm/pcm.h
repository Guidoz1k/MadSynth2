#ifndef __PCM_H
#define __PCM_H

#include <Arduino.h>
#include <arm_math.h> // for float32_t type

// macros

#define INTPIN 2    // little green LED with a 100Ω resistor 

#define BCLK    21
#define WCLK    20
#define DOUT    7

// variables

typedef enum {
    DDS         = 0,
    PHYS_STRING = 1,
} synth_mode_t;

typedef enum {
    SINE    = 0,
    SQUARE  = 1,
    TRIANG  = 2,
    SAW     = 3,
    INV_SAW = 4,
} waveform_t;

// functions

uint32_t return_dds_time(void);

void pcm_setup(void);

void set_DDSfrequency(float32_t freq);

void set_DDSwaveform(waveform_t waveform_r, waveform_t waveform_l);

void set_volume(float32_t vol);

#endif /* __PCM_H */