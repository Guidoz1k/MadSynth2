#include "pcm.h"

#include <Audio.h>

#include "lut.h"

IntervalTimer timer;

AudioPlayQueue           leftqueue;
AudioPlayQueue           rightqueue;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(leftqueue, 0, i2s1, 0);
AudioConnection          patchCord2(rightqueue, 0, i2s1, 1);

float32_t increment;
float32_t volume;

void set_volume(float vol){
    volume = vol;
}

void frequency(uint16_t freq){
    increment = (float32_t)freq * (float32_t)((float32_t)2048 / (float32_t)44100);
}

void i2s_setup(void){
    pinMode(INTPIN, OUTPUT);
	AudioMemory(64);
    timer.begin(timerInterrupt, 10000);

    frequency(450);
    set_volume(0.5);
}

// Timer interrupt service routine (ISR) 10ms to fill 441 positions in buffer
void timerInterrupt(){
    int16_t buffer_l[441];
    int16_t buffer_r[441];
    uint16_t i;
    static float32_t counter = 0;

    digitalWriteFast(INTPIN, HIGH);

    for(i = 0; i < 441; i++){
        buffer_r[i] = (int16_t)(lut_sin[(uint16_t)counter] * volume);
        buffer_l[i] = (int16_t)(lut_sin[(uint16_t)counter] * volume);
        counter += increment;
        if(counter >= 2048)
            counter -= 2048;
    }

    leftqueue.play(buffer_l, 441);
    rightqueue.play(buffer_r, 441);

    digitalWriteFast(INTPIN, LOW);
}
