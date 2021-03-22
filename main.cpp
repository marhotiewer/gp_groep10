#include <wiringPi.h>
#include <iostream>
#include <string>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    wiringPiSetup();
    pinMode(1, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(1920);
    pwmSetRange(200);
    pwmWrite(1, 13);

    httplib::Server svr;
    svr.Post("/steer", [](const httplib::Request &req, httplib::Response &res) {
        std::cout << "(" << req.remote_addr << ") 200 OK: " << req.body << std::endl;
        auto data = json::parse(req.body);
        pwmWrite(1, data["angle"]);
    });

    svr.listen("0.0.0.0", 80);
    return 0;
}
