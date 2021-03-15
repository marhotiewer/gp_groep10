#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <pigpio.h>

int main(){
        if (gpioInitialise() < 0){
                return -1;
        }
        while(true){
        	gpioServo(18, 1000);
		sleep(2);
		gpioServo(18, 2000);
		sleep(2);
        }
}
