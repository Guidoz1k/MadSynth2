#include <Arduino.h>
#include "pcm.h"

void setup(){
	pinMode(LED_BUILTIN, OUTPUT);
    i2s_setup();
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
