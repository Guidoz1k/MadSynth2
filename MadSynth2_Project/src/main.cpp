#include <Arduino.h>

#define TESTPIN 24

IntervalTimer timer;

const unsigned long timerInterval = 250000;

// Timer interrupt service routine (ISR)
void timerInterruptHandler(){
	static int8_t status = 0;

	if(status == 1){
		status = 0;
		digitalWriteFast(LED_BUILTIN, HIGH);
	}
	else{
		status = 1;
		digitalWriteFast(LED_BUILTIN, LOW);
	}
	
}

void setup(){
	pinMode(TESTPIN, INPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	Serial.begin(115200);

	timer.begin(timerInterruptHandler, timerInterval);
	delay(2000);
	Serial.println("SETUP COMPLETE");
}

void loop(){
	if(digitalReadFast(TESTPIN) == HIGH){
		Serial.println("BUTTON");
		delay(1000);
	}
}
