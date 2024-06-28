#ifndef __SYNTH_DDS_H
#define __SYNTH_DDS_H

#include <Arduino.h>
#include <arm_math.h> // for float32_t type

typedef enum {
    WAVE_SINE    = 0,
    WAVE_SQUARE  = 1,
    WAVE_TRIANG  = 2,
    WAVE_SAW     = 3,
    WAVE_INV_SAW = 4,
    WAVE_NONE    = 5,   // used to skip selection
} waveform_t;

void DDS_mode(uint8_t sample_size, int16_t *r_channel, int16_t *l_channel);

void set_DDSwaveform(waveform_t waveform_r, waveform_t waveform_l);

void set_DDSoscillator(uint8_t key);

void clear_DDSoscillator(uint8_t key);

#endif /* __SYNTH_DDS_H */