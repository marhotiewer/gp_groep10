#include <wiringPi.h>
#include <stdio.h>

int main (void)
{
   printf ("Raspberry Pi wiringPi test program\n");
   wiringPiSetupGpio();
   pinMode(1, PWM_OUTPUT) ;
   pwmSetMode(PWM_MODE_MS);
   pwmSetRange(2000);
   pwmSetClock(192);
   pwmWrite(1,110);
   delay(1000);
   pwmWrite(1,190);
   return 0;
}
