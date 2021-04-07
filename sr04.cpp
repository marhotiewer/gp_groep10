#include <wiringPi.h>
#include <iostream>

int sr04(){
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
	return ((end - begin) / 2) / 29;
}

int main(int argc, char* argv[]) {
	wiringPiSetup();
	pinMode(5, OUTPUT);
	pinMode(4, INPUT);
	pullUpDnControl(4, PUD_DOWN);

	for(;;) {
		std::cout << "Afstand: " << sr04() << "cm\n";
		delay(250);
	}
}
