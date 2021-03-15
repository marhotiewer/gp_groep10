#include <wiringPi.h>
#include <iostream>
#include "httplib.h"

int main(int argc, char* argv[]) {
    httplib::Server svr;

    wiringPiSetup();
    pinMode(17, PWM_OUTPUT);
    
    for(;;) {
        for(int i = 0; i < 1024; i++) {
            pwmWrite(17, i);
        }
    }

    svr.Get("/on", [](const httplib::Request &, httplib::Response &res) {
          res.set_content("ON", "text/plain");
          std::cout << "ON\n";
          digitalWrite(0, HIGH); delay(500);
    });

    svr.Get("/off", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("OFF", "text/plain");
        std::cout << "OFF\n";
        digitalWrite(0, LOW); delay(500);
    });

    svr.listen("0.0.0.0", 8080);
    return 0;
}

