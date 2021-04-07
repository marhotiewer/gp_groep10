#include <wiringPi.h>
#include <iostream>
#include <thread>
#include <functional>

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

void getDistance(int &UwU){
	for(;;){
		UwU = sr04();
		delay(100);
	}
}



int main(int argc, char* argv[]) {
	wiringPiSetup();
	pinMode(5, OUTPUT);
	pinMode(4, INPUT);
	pinMode(15, OUTPUT);
	pullUpDnControl(4, PUD_DOWN);
	
	int distance = 0;
	std::thread thread_1(getDistance, std::ref(distance)); 
	
	for(;;) {
		std::cout << distance << std::endl;
		if(distance < 20 && distance >= 10){
			digitalWrite(15, HIGH);
			delay(500);
			digitalWrite(15, LOW);
		}
		else if(distance < 10 && distance >= 5){
			digitalWrite(15, HIGH);
			delay(200);
			digitalWrite(15, LOW);
		}
		else if(distance < 5 && distance >= 1){
			digitalWrite(15, HIGH);
		}
		else{
			digitalWrite(15, LOW);
		}
		delay(500);
	}
}