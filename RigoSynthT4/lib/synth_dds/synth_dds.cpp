#include "synth_dds.h"

#include "lut.h"    // LUT collection
#include "keys.h"

// =============================================== variables ===============================================

struct {
    bool state[MAXKEYS] = {0};
    uint32_t dds_counter[MAXKEYS] = {0};
    uint32_t increment[MAXKEYS] = {0};

    waveform_t waveform_r = WAVE_SINE;
    waveform_t waveform_l = WAVE_SINE;
    uint8_t harmonics_r = 1;
    uint8_t harmonics_l = 1;
    uint16_t active_oscs = 0;
} oscillators;

// dds variables
int16_t wave_buffer_r[LUTSIZE] = {0};
int16_t wave_buffer_l[LUTSIZE] = {0};

// =============================================== INTERRUPT ===============================================

void DDS_mode(uint8_t sample_size, int16_t *r_channel, int16_t *l_channel){
    uint8_t i, j;

    for(i = 0; i < MAXKEYS; i++){
        if(oscillators.state[i] == true){
            for(j = 0; j < sample_size; j++){
                r_channel[j] += wave_buffer_r[ oscillators.dds_counter[i] >> 20] / oscillators.active_oscs;
                l_channel[j] += wave_buffer_l[ oscillators.dds_counter[i] >> 20] / oscillators.active_oscs;
                oscillators.dds_counter[i] += oscillators.increment[i];
                oscillators.dds_counter[i] &= 0x7FFFFFFF; // 0x07FF but for the <<20
            }
        }
    }
}

// =============================================== functions ===============================================

void set_DDSwaveform_r(waveform_t wave_r){
    uint16_t i, j;
    int16_t *temp_pointer_r = lut_sin;

    switch(wave_r){
    case WAVE_SINE:
        oscillators.waveform_r = WAVE_SINE;
        temp_pointer_r = lut_sin;
        break;
    case WAVE_SQUARE:
        oscillators.waveform_r = WAVE_SQUARE;
        temp_pointer_r = lut_squ;
        break;
    case WAVE_TRIANG:
        oscillators.waveform_r = WAVE_TRIANG;
        temp_pointer_r = lut_tri;
        break;
    case WAVE_SAW:
        oscillators.waveform_r = WAVE_SAW;
        temp_pointer_r = lut_saw;
        break;
    case WAVE_INV_SAW:
        oscillators.waveform_r = WAVE_INV_SAW;
        temp_pointer_r = lut_inv;
        break;
    default:
        oscillators.waveform_r = WAVE_SINE;
        temp_pointer_r = lut_sin;
        break;
    }
    for(i = 0; i < LUTSIZE; i++)
        wave_buffer_r[i] = 0;
    for(i = 1; i <= oscillators.harmonics_r; i++){
        for(j = 0; j < LUTSIZE; j++){
            wave_buffer_r[j] += temp_pointer_r[(j * i) & 0x07FF] / oscillators.harmonics_r;
        }
    }
}

void set_DDSwaveform_l(waveform_t wave_l){
    uint16_t i, j;
    int16_t *temp_pointer_l = lut_sin;

    switch(wave_l){
    case WAVE_SINE:
        oscillators.waveform_l = WAVE_SINE;
        temp_pointer_l = lut_sin;
        break;
    case WAVE_SQUARE:
        oscillators.waveform_l = WAVE_SQUARE;
        temp_pointer_l = lut_squ;
        break;
    case WAVE_TRIANG:
        oscillators.waveform_l = WAVE_TRIANG;
        temp_pointer_l = lut_tri;
        break;
    case WAVE_SAW:
        oscillators.waveform_l = WAVE_SAW;
        temp_pointer_l = lut_saw;
        break;
    case WAVE_INV_SAW:
        oscillators.waveform_l = WAVE_INV_SAW;
        temp_pointer_l = lut_inv;
        break;
    default:
        oscillators.waveform_l = WAVE_SINE;
        temp_pointer_l = lut_sin;
        break;
    }
    for(i = 0; i < LUTSIZE; i++)
        wave_buffer_l[i] = 0;
    for(i = 1; i <= oscillators.harmonics_l; i++){
        for(j = 0; j < LUTSIZE; j++){
            wave_buffer_l[j] += temp_pointer_l[(j * i) & 0x07FF] / oscillators.harmonics_l;
        }
    }
}

void set_DDSharmonics_r(uint8_t harm_r){
    if(harm_r > 0)
        oscillators.harmonics_r = harm_r;
    set_DDSwaveform_r(oscillators.waveform_r);
}

void set_DDSharmonics_l(uint8_t harm_l){
    if(harm_l > 0)
        oscillators.harmonics_l = harm_l;
    set_DDSwaveform_l(oscillators.waveform_l);
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

void DDS_setup(void){
    set_DDSwaveform_r(WAVE_SINE);
    set_DDSwaveform_l(WAVE_SINE);
}
