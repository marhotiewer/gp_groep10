#include <wiringPi.h>
#include <iostream>
#include <chrono>

void sr04(){
	wiringPiSetup();
	pinMode(5, OUTPUT);
	pinMode(4, INPUT);
	pullUpDnControl(4, PUD_DOWN);
	
	digitalWrite(5, HIGH);
	delay(1);
	digitalWrite(5, LOW);

    unsigned int start = 0;
    unsigned int end = 0;

	while(digitalRead(4) == LOW){
        start = millis();
	}
	while(digitalRead(4) == HIGH){
        end = millis();
	}
	
	std::cout << "Afstand: " << end - begin * 17165000 << "cm\n";
}

int main(int argc, char* argv[]) {
	for(;;) {
		sr04();
		delay(500);
	}
}

