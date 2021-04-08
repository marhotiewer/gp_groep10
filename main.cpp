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
	// Send a short pulse to the sensor to have it measure the data and send us it
	digitalWrite(SR04_WRITE, HIGH);
	delay(1);
	digitalWrite(SR04_WRITE, LOW);

    unsigned int begin = 0;
    unsigned int end = 0;
	
	// Measure the amount of time the signal is low for
	while(digitalRead(SR04_READ) == LOW){
        begin = micros();
	}
	while(digitalRead(SR04_READ) == HIGH){
        end = micros();
	}
	// This magic equation will give us the distance in cm from the amount of time SR04_READ was LOW
	return ((end - begin) / 2) / 29;
}

void get_distance(int &distance) {
	for(;;){
		distance = read_sr04();
		
		// Get the distance from an object and depending on the result beep the speaker
		if(distance < 4) {
			digitalWrite(SPEAKER, HIGH);
			delay(50);
			digitalWrite(SPEAKER, LOW);
			delay(50);
		}
		if(distance < 8) {
			digitalWrite(SPEAKER, HIGH);
			delay(125);
			digitalWrite(SPEAKER, LOW);
			delay(125);
		}
		else if(distance < 15) {
			digitalWrite(SPEAKER, HIGH);
			delay(250);
			digitalWrite(SPEAKER, LOW);
			delay(250);
		}
		else if(distance < 25) {
			digitalWrite(SPEAKER, HIGH);
			delay(500);
			digitalWrite(SPEAKER, LOW);
			delay(500);
		}
		else {
			digitalWrite(SPEAKER, HIGH);
			delay(500);
		}
	}
}

int main(int argc, char* argv[]) {
	// Do some initialization
	httplib::Server svr;
	int distance = 0;
    wiringPiSetup();
	
	// Prepare steering servo and set the wheels straight
	// Set the pulse width to 20ms for our servo model
    pinMode(SERVO_STR_PWM, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(SERVO_PWM_CLOCK);
    pwmSetRange(SERVO_PWM_RANGE);
	// A duty cycle of 1ms to 2ms will give us 0 to 180 degrees rotation, initialize the wheels to be centered
    pwmWrite(SERVO_STR_PWM, 13);
	
	// Prepare software pwm for throttle and set it forward
	// Our motor doesn't care about the pulse length, only the duty cycle, so here we'll use software pwm with a duty cycle from 0 to 100%
	softPwmCreate(MOTOR_PWM, 0, 100);
	pinMode(DIRECTIONAL_1, OUTPUT);
	pinMode(DIRECTIONAL_2, OUTPUT);
	// We can control the direction of the motor by flipping these two pins, initialize it to forward
	digitalWrite(DIRECTIONAL_1, HIGH);
	digitalWrite(DIRECTIONAL_2, LOW);
	
	// Prepare parking sensor
	// Make sure the pull down resister on our input is enabled so the signal wont be undefined
	pullUpDnControl(SR04_READ, PUD_DOWN);
    pinMode(SR04_WRITE, OUTPUT);
	pinMode(SR04_READ, INPUT);
	pinMode(SPEAKER, OUTPUT);
	
	// Start the thread for the parking sensor
	std::thread main_thread(get_distance, std::ref(distance));
	
	// Bind webserver endpoints
	// Read the webpage from disk and serve it to the user to allow control of the vehicle
	svr.Get("/", [](const httplib::Request &req, httplib::Response &res) {
        std::ifstream ifs("control.html");
        std::string control_src((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        res.set_content(control_src, "text/html");
		ifs.close();
    });
	// The data for every request gets sent in json, in the following lambda functions we decode the data and control the components
    svr.Post("/steer", [](const httplib::Request &req, httplib::Response &res) {
        std::cout << "(" << req.remote_addr << ") 200 OK: " << req.body << std::endl;
        auto data = json::parse(req.body);
        pwmWrite(SERVO_STR_PWM, data["angle"]);
        res.set_content(req.body, "application/json");
    });
	svr.Post("/throttle", [&distance](const httplib::Request &req, httplib::Response &res) {
        std::cout << "(" << req.remote_addr << ") 200 OK: " << req.body << std::endl;
		auto data = json::parse(req.body);
		
		// If direction is true go backwards, otherwise, default to forward
		if(data["direction"]) {
			digitalWrite(DIRECTIONAL_1, LOW);
			digitalWrite(DIRECTIONAL_2, HIGH);
		}
		else {
			digitalWrite(DIRECTIONAL_1, HIGH);
			digitalWrite(DIRECTIONAL_2, LOW);
		}
		// If the parking sensor isn't within 5cm the motor will drive the wheel
		if(distance <! 5) {
			softPwmWrite(MOTOR_PWM, data["speed"]);
		}
        res.set_content(req.body, "application/json");
    });
	// Start listening for incoming API requests on port 80, this will block the thread
    svr.listen("0.0.0.0", 80);
    return 0;
}
