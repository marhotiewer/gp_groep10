#include <wiringPi.h>
#include <iostream>
#include <fstream>
#include <string>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

#define SERVO_STR_PWM	1
#define SERVO_PWM_CLOCK	1920
#define SERVO_PWM_RANGE	200

int main(int argc, char* argv[]) {
	httplib::Server svr;
    wiringPiSetup();
	
	// Prepare steering servo and set the wheels straight
    pinMode(SERVO_STR_PWM, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(SERVO_PWM_CLOCK);
    pwmSetRange(SERVO_PWM_RANGE);
    pwmWrite(SERVO_STR_PWM, 13);
    
	// Bind webserver endpoints
	svr.Get("/", [](const httplib::Request &req, httplib::Response &res) {
        std::ifstream ifs("control.html");
        std::string control_src((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        res.set_content(control_src, "text/html");
		ifs.close();
    });
    svr.Post("/steer", [](const httplib::Request &req, httplib::Response &res) {
        std::cout << "(" << req.remote_addr << ") 200 OK: " << req.body << std::endl;
        auto data = json::parse(req.body);
        pwmWrite(SERVO_STR_PWM, data["angle"]);
        res.set_content(req.body, "application/json");
    });
	
	// Start listening on port 80, this will block the thread
    svr.listen("0.0.0.0", 80);
    return 0;
}
