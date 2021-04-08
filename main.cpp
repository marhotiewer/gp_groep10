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

#define MOTOR_PWM		4
#define DIRECTIONAL_2	6
#define DIRECTIONAL_1	5

#define SR04_WRITE		12
#define	SR04_READ		13

#define	SPEAKER			2

int read_sr04() {
	digitalWrite(SR04_WRITE, HIGH);
	delay(1);
	digitalWrite(SR04_WRITE, LOW);

    unsigned int begin = 0;
    unsigned int end = 0;

	while(digitalRead(SR04_READ) == LOW){
        begin = micros();
	}
	while(digitalRead(SR04_READ) == HIGH){
        end = micros();
	}
	return ((end - begin) / 2) / 29;
}

void get_distance(int &distance) {
	for(;;){
		distance = read_sr04();
		
		if(distance < 5) {
			digitalWrite(SPEAKER, HIGH);
			delay(25);
			digitalWrite(SPEAKER, LOW);
			delay(25);
		}
		else if(distance < 10) {
			digitalWrite(SPEAKER, HIGH);
			delay(50);
			digitalWrite(SPEAKER, LOW);
			delay(50);
		}
		else if(distance < 15) {
			digitalWrite(SPEAKER, HIGH);
			delay(100);
			digitalWrite(SPEAKER, LOW);
			delay(100);
		}
		else {
			delay(100);
		}
	}
}

int main(int argc, char* argv[]) {
	httplib::Server svr;
	int distance = 0;
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
	digitalWrite(DIRECTIONAL_1, HIGH);
	digitalWrite(DIRECTIONAL_2, LOW);
	
	// Prepare parking sensor system
	pullUpDnControl(SR04_READ, PUD_DOWN);
    pinMode(SR04_WRITE, OUTPUT);
	pinMode(SR04_READ, INPUT);
	pinMode(SPEAKER, OUTPUT);
	
	std::thread main_thread(get_distance, std::ref(distance));
	
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
	svr.Post("/throttle", [&distance](const httplib::Request &req, httplib::Response &res) {
        std::cout << "(" << req.remote_addr << ") 200 OK: " << req.body << std::endl;
		auto data = json::parse(req.body);
		
		if(data["direction"]) {
			digitalWrite(DIRECTIONAL_1, LOW);
			digitalWrite(DIRECTIONAL_2, HIGH);
		}
		else {
			digitalWrite(DIRECTIONAL_1, HIGH);
			digitalWrite(DIRECTIONAL_2, LOW);
		}
		if(distance <! 5) {
			softPwmWrite(MOTOR_PWM, data["speed"]);
		}
        res.set_content(req.body, "application/json");
    });
	// Start listening on port 80, this will block the thread
    svr.listen("0.0.0.0", 80);
    return 0;
}
