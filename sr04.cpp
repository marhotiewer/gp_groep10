#include <wiringPi.h>
#include <chrono>
#include <iostream>
#include <thread>

void sr04(){
	wiringPiSetup();
	pinMode(5, OUTPUT);
	pinMode(4, INPUT);
	pullUpDnControl(4, PUD_DOWN);
	
	digitalWrite(5, HIGH);
	delay(1);
	digitalWrite(5, LOW);

	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	
	while(digitalRead(4) == LOW){
		begin = std::chrono::steady_clock::now();
	}
	while(digitalRead(4) == HIGH){
		end = std::chrono::steady_clock::now();
	}
	
	std::cout << "Afstand: " << std::chrono::duration_cast<std::chrono::seconds>((end - begin)*17165).count() << " cm\n";
}


int main(){
	while(true){
		sr04();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
