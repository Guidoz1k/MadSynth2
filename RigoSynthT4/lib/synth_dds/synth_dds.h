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

void set_DDSwaveform_r(waveform_t waveform_r);

void set_DDSwaveform_l(waveform_t waveform_l);

void set_DDSharmonics_r(uint8_t harm_r);

void set_DDSharmonics_l(uint8_t harm_l);

void set_DDSoscillator(uint8_t key);

void clear_DDSoscillator(uint8_t key);

void DDS_setup(void);

#endif /* __SYNTH_DDS_H */