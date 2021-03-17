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
    svr.Post("/steer", [](const httplib::Request &req, httplib::Response &res) {
        std::cout << "(" << req.remote_addr << ") 200 OK: " << req.body << std::endl;
    });

    svr.listen("0.0.0.0", 8080);
    return 0;
}

