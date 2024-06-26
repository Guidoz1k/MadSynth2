#include "pcm.h"

// audio api library
#include <Audio.h>
#include <utility/imxrt_hw.h> // for the I²S frequency change

// LUT collection
#include "lut.h"

#define SAMPLERATE 192000

// =============================================== variables ===============================================

// interrupt variables
IntervalTimer timer;
synth_mode_t int_mode = DDS;
uint32_t interrupt_counter = 0; // incremented every subdivision of the 10ms total interrupt time. Right now, 1ms

// audio api variables
AudioPlayQueue           leftqueue;
AudioPlayQueue           rightqueue;
AudioOutputI2S           i2s1;
AudioConnection          patchCord2(rightqueue, 0, i2s1, 0);
AudioConnection          patchCord1(leftqueue, 0, i2s1, 1);

// synth variables
float32_t volume = 0.5;

// dds variables
uint32_t increment = 0;
int16_t *waveform_pointer_r = lut_sin;
int16_t *waveform_pointer_l = lut_sin;

/* =============================================== INTERRUPT ===============================================
    Timer ISR should add 441 positions on each queue every 10ms, for 44.1kHz
    By filling 440 positions in subdivisions and adding 1 extra in the last one, it is
    possible to use smaller interrupt periods. Right now it is doing 10 subdivisions,
    loading 44 samples to the I²S bus in the first 9 and 45 samples in the last one.
    As it is 10 subdivisions on a 10ms period, subdivisions will occur every 1ms.
*/

void DDS_mode(uint8_t sample_size, int16_t *r_channel, int16_t *l_channel){
    uint8_t i;
    static uint32_t dds_counter = 0;

    for(i = 0; i < sample_size; i++){
        r_channel[i] = waveform_pointer_r[dds_counter >> 20];
        l_channel[i] = waveform_pointer_l[dds_counter >> 20];
        dds_counter += increment;
        dds_counter &= 0x7FFFFFFF; // 0x07FF but for the <<20
    }
}

void PHYS_STRING_mode(uint8_t sample_size, int16_t *right_channel, int16_t *left_channel){

}

void timer_interrupt(){
    int16_t buffer_r[192];
    int16_t buffer_l[192];

    // external monitoring for interrupt processing time
    digitalWriteFast(INTPIN, HIGH);

    interrupt_counter++; // a 1ms counter for the system

    // different synthesis modes are switched here
    switch(int_mode){
    case DDS:
        DDS_mode(192, buffer_r, buffer_l);
        break;
    case PHYS_STRING:
        PHYS_STRING_mode(192, buffer_r, buffer_l);
        break;
    default:
        break;
    }

    // load buffers in the I²S peripheral
    leftqueue.play(buffer_l, 192);
    rightqueue.play(buffer_r, 192);

    // external monitoring for interrupt processing time
    digitalWriteFast(INTPIN, LOW);
}

// =============================================== functions ===============================================

uint32_t return_dds_time(void){
    return interrupt_counter;
}

void set_volume(float32_t vol){
    volume = vol;
}

void set_DDSfrequency(float32_t freq){ // provisory
    increment = (uint32_t)((freq * 1024 * 1024) * ((float32_t)LUTSIZE/(float32_t)SAMPLERATE)); // *1024 because is the equivalent of << 20 on the interrupt
}

void set_DDSwaveform(waveform_t waveform_r, waveform_t waveform_l){
    switch(waveform_r){
    case SINE:
        waveform_pointer_r = lut_sin;
        break;
    case SQUARE:
        waveform_pointer_r = lut_squ;
        break;
    case TRIANG:
        waveform_pointer_r = lut_tri;
        break;
    case SAW:
        waveform_pointer_r = lut_saw;
        break;
    case INV_SAW:
        waveform_pointer_r = lut_inv;
        break;
    default:
        break;
    }
    switch(waveform_l){
    case SINE:
        waveform_pointer_l = lut_sin;
        break;
    case SQUARE:
        waveform_pointer_l = lut_squ;
        break;
    case TRIANG:
        waveform_pointer_l = lut_tri;
        break;
    case SAW:
        waveform_pointer_l = lut_saw;
        break;
    case INV_SAW:
        waveform_pointer_l = lut_inv;
        break;
    default:
        break;
    }

}

// https://forum.pjrc.com/index.php?threads/change-sample-rate-for-teensy-4-vs-teensy-3.57283/
void setI2SFreq(int freq) {
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (freq * 256 * n1);
  double C = ((double)freq * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, true);
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
       | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
       | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f
}

void pcm_setup(void){
    pinMode(INTPIN, OUTPUT);
	AudioMemory(64);
    timer.begin(timer_interrupt, 1000); // 1000µs = 1ms tick rate for the interrupt
    setI2SFreq(SAMPLERATE); // sample rate = 192kHz
}

