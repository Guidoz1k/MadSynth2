#include "synth_dds.h"

#include "lut.h"    // LUT collection
#include "keys.h"

// =============================================== variables ===============================================

struct {
    bool state[MAXKEYS] = {0};
    uint32_t dds_counter[MAXKEYS] = {0};
    uint32_t increment[MAXKEYS] = {0};

    uint16_t active_oscs = 0;
} oscillators;

// dds variables
int16_t *waveform_pointer_r = lut_sin;
int16_t *waveform_pointer_l = lut_sin;

// =============================================== INTERRUPT ===============================================

void DDS_mode(uint8_t sample_size, int16_t *r_channel, int16_t *l_channel){
    uint8_t i, j;

    for(i = 0; i < MAXKEYS; i++){
        if(oscillators.state[i] == true){
            for(j = 0; j < sample_size; j++){
                r_channel[j] += waveform_pointer_r[ oscillators.dds_counter[i] >> 20] / oscillators.active_oscs;
                l_channel[j] += waveform_pointer_l[ oscillators.dds_counter[i] >> 20] / oscillators.active_oscs;
                oscillators.dds_counter[i] += oscillators.increment[i];
                oscillators.dds_counter[i] &= 0x7FFFFFFF; // 0x07FF but for the <<20
            }
        }
    }
}

// =============================================== functions ===============================================

void set_DDSwaveform(waveform_t waveform_r, waveform_t waveform_l){
    switch(waveform_r){
    case WAVE_SINE:
        waveform_pointer_r = lut_sin;
        break;
    case WAVE_SQUARE:
        waveform_pointer_r = lut_squ;
        break;
    case WAVE_TRIANG:
        waveform_pointer_r = lut_tri;
        break;
    case WAVE_SAW:
        waveform_pointer_r = lut_saw;
        break;
    case WAVE_INV_SAW:
        waveform_pointer_r = lut_inv;
        break;
    default:
        break;
    }
    switch(waveform_l){
    case WAVE_SINE:
        waveform_pointer_l = lut_sin;
        break;
    case WAVE_SQUARE:
        waveform_pointer_l = lut_squ;
        break;
    case WAVE_TRIANG:
        waveform_pointer_l = lut_tri;
        break;
    case WAVE_SAW:
        waveform_pointer_l = lut_saw;
        break;
    case WAVE_INV_SAW:
        waveform_pointer_l = lut_inv;
        break;
    default:
        break;
    }
}

void set_DDSoscillator(uint8_t key){
    // *1024 because is the equivalent of << 20 on the interrupt
    // 192000 is the sample rate
    oscillators.increment[key] = (uint32_t)((key_frequencies[key] * 1024 * 1024) * ((float32_t)LUTSIZE/(float32_t)192000));
    oscillators.dds_counter[key] = 0;
    oscillators.state[key] = true;
    oscillators.active_oscs++;
}

void clear_DDSoscillator(uint8_t key){
    oscillators.increment[key] = 0;
    oscillators.dds_counter[key] = 0;
    oscillators.state[key] = false;
    oscillators.active_oscs--;
}
