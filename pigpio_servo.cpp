#include <iostream>
#include <conio.h>

#include <pigpio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


using namespace std;

int main()
{
    char ch=0;
    cout << "Press Q to quit\n";
    do
    {
        ch = getch();

        switch(ch)
        {
            case 'W':
            case 'w':
                cout << "W was pressed \n";
                break;
            case 'A':
            case 'a':
                cout << "A was pressed \n";
                gpioInitialise();
		gpioServo(18, 2000);
		sleep(0.3);
		break;
            case 's':
            case 'S':
                cout << "S was pressed \n";
                break;
            case 'D':
            case 'd':
		cout << "D was pressed \n";
                gpioInitialise();
		gpioServo(18, 1000);
                sleep(0.3);
		break;
	    default:
		cout << "Dikke kut zooi\n";
		gpioInitialise();
		gpioServo(18, 1500);
        }

    }while (ch != 'Q' && ch!='q');
}
