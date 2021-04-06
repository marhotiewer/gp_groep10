#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>
#include <fstream>
#include <string>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

#define SERVO_STR_PWM	1
#define SERVO_PWM_CLOCK	1920
#define SERVO_PWM_RANGE	200

#define MOTOR_PWM		7
#define DIRECTIONAL_2	6
#define DIRECTIONAL_1	5

int main(int argc, char* argv[]) {
	httplib::Server svr;
    wiringPiSetup();
	
	// Prepare steering servo and set the wheels straight
    pinMode(SERVO_STR_PWM, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(SERVO_PWM_CLOCK);
    pwmSetRange(SERVO_PWM_RANGE);
    pwmWrite(SERVO_STR_PWM, 13);
	
	// Prepare software pwm for throttle and set it forward
	softPwmCreate(MOTOR_PWM, 0, 100);
	pinMode(DIRECTIONAL_1, OUTPUT);
	pinMode(DIRECTIONAL_2, OUTPUT);
	pinWrite(DIRECTIONAL_1, HIGH);
	pinWrite(DIRECTIONAL_1, LOW);
    
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
	svr.Post("/throttle", [](const httplib::Request &req, httplib::Response &res) {
        std::cout << "(" << req.remote_addr << ") 200 OK: " << req.body << std::endl;
		auto data = json::parse(req.body);
        softPwmWrite(MOTOR_PWM, data["speed"]);
        res.set_content(req.body, "application/json");
    });
	// Start listening on port 80, this will block the thread
    svr.listen("0.0.0.0", 80);
    return 0;
}
