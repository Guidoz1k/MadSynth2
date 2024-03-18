#include <Arduino.h>

#define PINLED 24

IntervalTimer timer;

// Define the interval for the timer interrupt in microseconds
const unsigned long timerInterval = 1000000; // 1 second

// Timer interrupt service routine (ISR)
void timerInterruptHandler(){
	static int status = 0;

	if(status == 1){
		status = 0;
		digitalWriteFast(LED_BUILTIN, HIGH);
	}
	else{
		status = 1;
		digitalWriteFast(LED_BUILTIN, LOW);
	}
	Serial.println("INTERRUPT");
}

void setup(){
	pinMode(PINLED, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	Serial.begin(115200);

	timer.begin(timerInterruptHandler, timerInterval);
	Serial.println("SETUP COMPLETE");
}

void loop(){
	/*
	digitalWriteFast(PINLED, HIGH);
	digitalWriteFast(LED_BUILTIN, LOW);
	delay(1000);
	digitalWriteFast(PINLED, LOW);
	digitalWriteFast(LED_BUILTIN, HIGH);
	delay(1000);
	*/
}
