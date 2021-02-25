#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "servo.h"

#define P_SERVO 1 //only wiringPi pin 1 (BCM_GPIO 18) supports PWM

int position = 0;		   //当前位置
int max_angle = 180;	   //最大角度
int servo_step = 1;		   //每一步的角度
int servo_step_delay = 20; //每一步的延时

void servoSetup()
{
	wiringPiSetup();
	pinMode(P_SERVO, PWM_OUTPUT); //only wiringPi pin 1 (BCM_GPIO 18) supports PWM
	pwmSetMode(PWM_MODE_MS);	  // Set to mode: mark:space
	pwmSetClock(500);			  //PWM clock: 19.2MHz, divisor 192:100KHz
	pwmSetRange(2000);			  //period 20ms
	servoRun(0);				  //恢复原位
}

void servoRun(int angle)
{
	if ((position + angle) > max_angle)
		angle = max_angle - position; //最大角度180

	if (angle > position)
	{
		for (int i = position; i <= angle; i += servo_step)
		{
			pwmWrite(P_SERVO, i);
			delay(servo_step_delay);
		}
	}
	else
	{
		for (int j = position; j >= angle; j -= servo_step)
		{
			pwmWrite(P_SERVO, j);
			delay(servo_step_delay);
		}
	}
	printf("servo from position:%d to %d\r\n",position,angle);
	position = angle;
}

void servoShutdown()
{
	pwmWrite(P_SERVO, 0);
}

int main(int argc, char *argv[])
{
	printf("starting\r\n");
	servoSetup();
	servoRun(45);
	delay(1000);
	servoRun(90);
	servoShutdown();
	printf("done\r\n");
	return 0;
}
