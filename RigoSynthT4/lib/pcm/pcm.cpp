#include "pcm.h"

// audio api library
#include <Audio.h>

// LUT collection
#include "lut.h"

// =============================================== variables ===============================================

// interrupt variable
IntervalTimer timer;

// audio api variables
AudioPlayQueue           leftqueue;
AudioPlayQueue           rightqueue;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(leftqueue, 0, i2s1, 0);
AudioConnection          patchCord2(rightqueue, 0, i2s1, 1);

// dds variables
uint32_t interrupt_counter = 0; // incremented every subdivision of the 10ms total interrupt time. Right now, 1ms
float32_t increment = 0;
float32_t volume = 0.1;

// =============================================== functions ===============================================

uint32_t return_dds_time(void){
    return interrupt_counter;
}

void set_volume(float vol){
    volume = vol;
}

void set_frequency(uint16_t freq){
    increment = (float32_t)freq * (float32_t)((float32_t)2048 / (float32_t)44100);
}

void i2s_setup(void){
    pinMode(INTPIN, OUTPUT);
	AudioMemory(64);
    timer.begin(timerInterrupt, 1000);

    set_frequency(20);
}

/* =============================================== INTERRUPT ===============================================
    Timer ISR should add 441 positions on each queue every 10ms, for 44.1kHz
    By filling 440 positions in subdivisions and adding 1 extra in the last one, it is
    possible to use smaller interrupt periods. Right now it is doing 10 subdivisions,
    loading 44 samples to the I²S bus in the first 9 and 45 samples in the last one.
    As it is 10 subdivisions on a 10ms period, subdivisions will occur every 1ms.
*/
void timerInterrupt(){
    uint16_t i;
    int16_t buffer_l[45];
    int16_t buffer_r[45];
    static float32_t dds_counter = 0;
    static uint8_t subperiod = 0;
    uint8_t subbuffer_size = (subperiod == 9) ? 45 : 44;

    // external monitoring for interrupt processing time
    digitalWriteFast(INTPIN, HIGH);

    interrupt_counter++; // a 1ms counter for the system

    for(i = 0; i < subbuffer_size; i++){
        buffer_r[i] = (int16_t)(lut_sin[(uint16_t)dds_counter] * volume);
        buffer_l[i] = (int16_t)(lut_sin[(uint16_t)dds_counter] * volume);
        dds_counter += increment;
        if(dds_counter >= 2048)
            dds_counter -= 2048;
    }

    // increments the subperiod counter
    subperiod++;
    if(subperiod > 9)
        subperiod = 0;

    // load buffers in the I²S peripheral
    leftqueue.play(buffer_l, subbuffer_size);
    rightqueue.play(buffer_r, subbuffer_size);

    // external monitoring for interrupt processing time
    digitalWriteFast(INTPIN, LOW);
}
