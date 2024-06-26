#include <Arduino.h>
#include "pcm.h"
#include "serial.h"
#include "keys.h"

void setup(){
	pinMode(LED_BUILTIN, OUTPUT);
    uarts_setup();
    pcm_setup();

    set_DDSfrequency(7980);
    set_DDSwaveform(SINE,SAW);
}

void loop(){
    static bool state = false;
    
    if(state == true){
        digitalWriteFast(LED_BUILTIN, LOW);
        state = false;
    }
    else{
        digitalWriteFast(LED_BUILTIN, HIGH);
        state = true;
    }

    delay(250);
}
