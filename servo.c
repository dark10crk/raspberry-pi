#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define P_SERVO 1 //only wiringPi pin 1 (BCM_GPIO 18) supports PWM

void setup()
{
        wiringPiSetup();
        pinMode(P_SERVO, PWM_OUTPUT); //only wiringPi pin 1 (BCM_GPIO 18) supports PWM
        pwmSetMode(PWM_MODE_MS);      // Set to mode: mark:space
        pwmSetClock(500);             //PWM clock: 19.2MHz, divisor 192:100KHz
        pwmSetRange(2000);            //period 20ms
        pwmWrite(P_SERVO, 0);       //go default position
}

int main(int argc, char *argv[])
{
        printf("starting\r\n");
        setup();
        delay(1000);
 
        for (int i = 0; i <= 180; i++)
        {
                pwmWrite(P_SERVO, i);
                delay(20);
        }

        for (int j = 180; j >= 0; j--)
        {
                pwmWrite(P_SERVO, j);
                delay(20);
        }
        printf("done\r\n");
        return 0;
}
