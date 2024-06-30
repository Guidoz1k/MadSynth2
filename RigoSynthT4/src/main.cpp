#include <Arduino.h>
#include "pcm.h"
#include "serial.h"
#include "umidi.h"

void setup(){
	pinMode(LED_BUILTIN, OUTPUT);
    uarts_setup();
    pcm_setup();
    uart_midi_setup();
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

    delay(100);
}
