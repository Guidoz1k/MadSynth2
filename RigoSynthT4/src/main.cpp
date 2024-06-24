#include <Arduino.h>
#include "pcm.h"

void setup(){
	pinMode(LED_BUILTIN, OUTPUT);
    i2s_setup();
}

void loop(){
    uint16_t i = 0;

    for(i = 20; i < 2000; i++){
        frequency(i);
        delay(2);
    }
    for(i = 2000; i > 20; i--){
        frequency(i);
        delay(2);
    }
}

/*
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
*/