#include <wiringPi.h>
#include <iostream>
#include <string>
#include "httplib.h"

int main(int argc, char* argv[]) {
    wiringPiSetup();
    pinMode(1, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(1920);
    pwmSetRange(200);
    pwmWrite(1, 13);

    while(false) {
	std::string input;
	std::cout << "Enter duty cycle (3-23): ";
        std::cin >> input;
	pwmWrite(1, std::stoi(input));
    }

    httplib::Server svr;
    svr.Get("/steer", [](const httplib::Request&, httplib::Response &res) {
        res.set_content("STEER", "text/plain");
        std::cout << "STEER\n";
    });

    svr.listen("0.0.0.0", 8080);
    return 0;
}

