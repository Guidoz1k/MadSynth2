#include "synth_dds.h"

#include "lut.h"    // LUT collection
#include "keys.h"

#define MAXALLOC 61

// ======================================== ADSR variables ========================================

typedef enum {
    ADSR_OFF        = 0,
    ADSR_ATTACK     = 1,    // transitory state that could occur after decay, sustain or release
    ADSR_DECAY      = 2,
    ADSR_SUSTAIN    = 3,
    ADSR_RELEASE    = 4,    // transitory state that could occur after attack, decay or sustain
} adsr_state_t;

uint_fast32_t adsr_counter[MAXALLOC] = {0};  // will always increase by 1 (ms)
adsr_state_t state[MAXALLOC] = {ADSR_OFF};

uint_fast32_t adsr_output_increment[MAXALLOC] = {0};
uint_fast32_t adsr_output[MAXALLOC] = {0};

// times in ms
uint_fast32_t attack_time = 0xFFFF;
uint_fast32_t decay_time = 0xFFFF;
//uint_fast16_t sustain_time = 1;
uint_fast32_t sustain_level = 0x7FFFFFFF;
uint_fast32_t release_time = 0xFFFF;

uint_fast8_t ADSR_channels_counter = 0;

// =============================================== DDS variables ===============================================

uint_fast8_t key_allocation[MAXKEYS];    // [NEEDS PRELOAD AT SETUP] stores which allocated slot has that key, 255 for not allocated
uint_fast8_t dds_allocation[MAXALLOC];   // [NEEDS PRELOAD AT SETUP] 255 for not allocated, 0 ~ 132 for MIDI key
uint_fast32_t dds_counter[MAXALLOC] = {0};
uint_fast32_t increment[MAXALLOC] = {0};

waveform_t waveform_r = WAVE_SINE;
waveform_t waveform_l = WAVE_SINE;
uint_fast8_t harmonics_r = 1;
uint_fast8_t harmonics_l = 1;

// dds variables
int_fast16_t wave_buffer_r[LUTSIZE] = {0};
int_fast16_t wave_buffer_l[LUTSIZE] = {0};

// ======================================== ADSR parameter functions ========================================

void set_ADSRattack(uint16_t time){
    if(time != 0)
        attack_time = time * 4;
}

void set_ADSRdecay(uint16_t time){
    if(time != 0)
        decay_time = time * 4;
}

/*
void set_ADSRsustain(uint16_t time){
    if(time != 0)
        sustain_time = time * 4;
}
*/

void set_ADSRsustain_level(uint16_t level){
    if(level != 0)
        sustain_level = level * 4;
}

void set_ADSRrelease(uint16_t time){
    if(time != 0)
        release_time = time * 4;
}

// ======================================== ADSR core functions ========================================

void trigger_ADSR(uint8_t adsr_unit, bool on_off){
    if(on_off == true){ // key pressed
        if(state[adsr_unit] == ADSR_OFF){ // ADSR was OFF and now it is on ATTACK
            ADSR_channels_counter++;
            adsr_output_increment[adsr_unit] = 0xFFFFFFFF / attack_time;
        }
        else{   // ADSR was already on and now it is resetted, transitioning to ATTACK
            adsr_output_increment[adsr_unit] = (0xFFFFFFFF - adsr_output[adsr_unit]) / attack_time;
        }
        adsr_counter[adsr_unit] = 0;
        state[adsr_unit] = ADSR_ATTACK;
    }
    else{   // key released
        adsr_counter[adsr_unit] = 0;
        state[adsr_unit] = ADSR_RELEASE;
        adsr_output_increment[adsr_unit] = adsr_output[adsr_unit] / release_time;
    }
}

/* iterate_ADSR
    - will iterate the adsr envelope
    - this is the function that will finish an ADSR envelope and decrease the ADSR_channels_counter
    - will also iterate ADSR envelopes that are off
*/
uint16_t iterate_ADSR(uint8_t adsr_unit){
    adsr_counter[adsr_unit]++;

    switch(state[adsr_unit]){
    case ADSR_OFF:
        adsr_output[adsr_unit] = 0;
        break;
    case ADSR_ATTACK:
        adsr_output[adsr_unit] += adsr_output_increment[adsr_unit];
        if(adsr_counter[adsr_unit] == attack_time){
            adsr_counter[adsr_unit] = 0;
            state[adsr_unit] = ADSR_DECAY;
            adsr_output_increment[adsr_unit] = (0xFFFFFFFF - sustain_level) / decay_time;
        }
        break;
    case ADSR_DECAY:
        adsr_output[adsr_unit] -= adsr_output_increment[adsr_unit];
        if(adsr_counter[adsr_unit] == decay_time){
            adsr_counter[adsr_unit] = 0;
            state[adsr_unit] = ADSR_SUSTAIN;
            adsr_output_increment[adsr_unit] = 0;
        }
        break;
    case ADSR_SUSTAIN:
        adsr_output[adsr_unit] = sustain_level; // nothing happens on sustain, the user must release the key
        break;
    case ADSR_RELEASE:
        adsr_output[adsr_unit] -= adsr_output_increment[adsr_unit];
        if(adsr_counter[adsr_unit] == release_time){
            adsr_counter[adsr_unit] = 0;
            state[adsr_unit] = ADSR_OFF;
            ADSR_channels_counter--;
            key_allocation[dds_allocation[adsr_unit]] = 255;
            dds_allocation[adsr_unit] = 255;    // stop allocation of oscillator
        }
        break;
    }

    return (adsr_output[adsr_unit] >> 16) / ADSR_channels_counter;
}

// =============================================== INTERRUPT ===============================================

void DDS_mode(uint8_t sample_size, int16_t *r_channel, int16_t *l_channel){
    uint_fast8_t i, j;
    uint_fast16_t temp_adsr;

    for(j = 0; j < sample_size; j++)
        for(i = 0; i < MAXALLOC; i++){
            temp_adsr = iterate_ADSR(i);
            r_channel[j] += ( wave_buffer_r[dds_counter[i] >> 20] * temp_adsr) >> 16;
            l_channel[j] += ( wave_buffer_l[dds_counter[i] >> 20] * temp_adsr) >> 16;
            dds_counter[i] += increment[i];
            dds_counter[i] &= 0x7FFFFFFF; // 0x07FF but for the <<20
        }

}

// =============================================== functions ===============================================

void set_DDSwaveform_r(waveform_t wave_r){
    uint16_t i, j;
    int16_t *temp_pointer_r = lut_sin;

    switch(wave_r){
    case WAVE_SINE:
        waveform_r = WAVE_SINE;
        temp_pointer_r = lut_sin;
        break;
    case WAVE_SQUARE:
        waveform_r = WAVE_SQUARE;
        temp_pointer_r = lut_squ;
        break;
    case WAVE_TRIANG:
        waveform_r = WAVE_TRIANG;
        temp_pointer_r = lut_tri;
        break;
    case WAVE_SAW:
        waveform_r = WAVE_SAW;
        temp_pointer_r = lut_saw;
        break;
    case WAVE_INV_SAW:
        waveform_r = WAVE_INV_SAW;
        temp_pointer_r = lut_inv;
        break;
    default:
        waveform_r = WAVE_SINE;
        temp_pointer_r = lut_sin;
        break;
    }
    for(i = 0; i < LUTSIZE; i++)
        wave_buffer_r[i] = 0;
    for(i = 1; i <= harmonics_r; i++){
        for(j = 0; j < LUTSIZE; j++){
            wave_buffer_r[j] += temp_pointer_r[(j * i) & 0x07FF] / harmonics_r;
        }
    }
}

void set_DDSwaveform_l(waveform_t wave_l){
    uint16_t i, j;
    int16_t *temp_pointer_l = lut_sin;

    switch(wave_l){
    case WAVE_SINE:
        waveform_l = WAVE_SINE;
        temp_pointer_l = lut_sin;
        break;
    case WAVE_SQUARE:
        waveform_l = WAVE_SQUARE;
        temp_pointer_l = lut_squ;
        break;
    case WAVE_TRIANG:
        waveform_l = WAVE_TRIANG;
        temp_pointer_l = lut_tri;
        break;
    case WAVE_SAW:
        waveform_l = WAVE_SAW;
        temp_pointer_l = lut_saw;
        break;
    case WAVE_INV_SAW:
        waveform_l = WAVE_INV_SAW;
        temp_pointer_l = lut_inv;
        break;
    default:
        waveform_l = WAVE_SINE;
        temp_pointer_l = lut_sin;
        break;
    }
    for(i = 0; i < LUTSIZE; i++)
        wave_buffer_l[i] = 0;
    for(i = 1; i <= harmonics_l; i++){
        for(j = 0; j < LUTSIZE; j++){
            wave_buffer_l[j] += temp_pointer_l[(j * i) & 0x07FF] / harmonics_l;
        }
    }
}

void set_DDSharmonics_r(uint8_t harm_r){
    if(harm_r > 0)
        harmonics_r = harm_r;
    set_DDSwaveform_r(waveform_r);
}

void set_DDSharmonics_l(uint8_t harm_l){
    if(harm_l > 0)
        harmonics_l = harm_l;
    set_DDSwaveform_l(waveform_l);
}

void start_DDSoscillator(uint8_t key){
    uint8_t allocator = 0;

    if(key_allocation[key] != 255){
        increment[key_allocation[key]] = (uint32_t)((key_frequencies[key] * 1024 * 1024) * ((float32_t)LUTSIZE/(float32_t)60000));
        trigger_ADSR(key_allocation[key], true);
    }
    else{
        while( (dds_allocation[allocator] != 255) && (allocator < MAXALLOC) )
            allocator++;
        if(allocator != MAXALLOC){
            key_allocation[key] = allocator;
            dds_allocation[allocator] = key;
            increment[allocator] = (uint32_t)((key_frequencies[key] * 1024 * 1024) * ((float32_t)LUTSIZE/(float32_t)60000));
            trigger_ADSR(allocator, true);
        }
    }
}

void stop_DDSoscillator(uint8_t key){
    trigger_ADSR(key_allocation[key], false);
}

void DDS_setup(void){
    uint8_t i;

    set_DDSwaveform_r(WAVE_SINE);
    set_DDSwaveform_l(WAVE_SINE);

    for(i = 0; i < MAXKEYS; i++){
        key_allocation[i] = 255;
    }
    for(i = 0; i < MAXALLOC; i++){
        dds_allocation[i] = 255;
    }
    
}
