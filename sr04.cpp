#include <wiringPi.h>
#include <iostream>

void sr04(){
	digitalWrite(5, HIGH);
	delay(1);
	digitalWrite(5, LOW);

    unsigned int begin = 0;
    unsigned int end = 0;

	while(digitalRead(4) == LOW){
        begin = micros();
	}
	while(digitalRead(4) == HIGH){
        end = micros();
	}
	
	std::cout << "Afstand: " << ((end - begin) / 2) / 29 << "cm\n";
}

int main(int argc, char* argv[]) {
	wiringPiSetup();
	pinMode(5, OUTPUT);
	pinMode(4, INPUT);
	pullUpDnControl(4, PUD_DOWN);
	
	for(;;) {
		sr04();
		delay(500);
	}
}
