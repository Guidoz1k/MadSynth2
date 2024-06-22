#include "main.h"

// Timer interrupt service routine (ISR)
void timerInterrupt(){
    static bool state = false;

    if(state == true){
        digitalWriteFast(LED_BUILTIN, LOW);
        digitalWriteFast(TESTPIN, HIGH);
        state = false;
    }
    else{
        digitalWriteFast(LED_BUILTIN, HIGH);
        digitalWriteFast(TESTPIN, LOW);
        state = true;

    }
}

void setup(){
    pinMode(TESTPIN, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	//Serial.begin(115200);

	timer.begin(timerInterrupt, 250000);
	//delay(2000);
	//Serial.println("SETUP COMPLETE");

    i2s_setup();
}

void loop(){
    delay(1000);
}
