/*********************************************************************/
/****************     robogame2017 作风优良组     ********************/
/****************            步进电机             ********************/
/*********************************************************************/

#include "stm32f10x.h"

/**********   引脚   ***********/
	 /******    GPIOA    ******/
#define ENABLE_1 	GPIO_Pin_0
#define MS1_1 		GPIO_Pin_6
#define MS2_1 		GPIO_Pin_2
#define MS3_1 		GPIO_Pin_3
#define STEP_1		GPIO_Pin_4
#define DIR_1		GPIO_Pin_5


/*********  步进模式  **********/
#define FULL_STEP       1
#define HALF_STEP       2
#define QUARTER_STEP    4
#define EIGHTH_STEP     8
#define SIXTEENTH_STEP  16

//一圈200步
//初始化步进电机
void Stepmotor_Configuration(void);
//设置步进电机1 每秒脉冲个数，并设置为长转模式
void setPulsePerSecond1(u32 pulse);
//设置步进电机1 每秒脉冲个数 和 步数（脉冲个数），并设置为计步模式
void setSteps1(u32 pulse,u32 number);
//设置步进电机1步进模式
void setResolution1(u8 resolution);
//停用步进电机
void disable1(void);							
//使能步进电机11	
void enable1(void);									
//设置步进电机1转向
void setDirection1(u8 dir);							
//72M时钟下，约延时us
void delay_nus(u32);
//72M时钟下，约延时ms
void delay_nms(u32);	
