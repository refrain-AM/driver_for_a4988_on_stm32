#include "stm32f10x.h"
#include "stdio.h"
#include "stepmotor.h"


void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}


//“ª»¶200≤Ω

int main(void)
{	
	//≤‚ ‘≥Ã–Ú
	Stepmotor_Configuration();
	setPulsePerSecond1(500);
	delay_nms(1000);
	setSteps1(1000,200);
	delay_nms(1000);
	setDirection1(0);
	setSteps1(300,200);
	delay_nms(1000);
	setPulsePerSecond1(500);
	delay_nms(1000);
	while(1)
	{ 
		setSteps1(1000,200);
		setDirection1(0);
		delay_nms(500);
		setSteps1(1000,200);
		setDirection1(1);
		delay_nms(500);
  }	
}	

