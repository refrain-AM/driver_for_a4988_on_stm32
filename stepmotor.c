#include "stepmotor.h"

#define YUZHI 99		//tim3计数器阈值

u8 flag1=0;  				//当flag为0，为长转模式；当flag为1，为计步模式
u32 counter1=0; 
u32 counter2=0; 
u32 jishi1=1000;		//每秒钟给 1M/(YUZHI+1)/jishi1 个脉冲
u32 jishi2=1000;
u32 maichongshu1=0;
u32 maichongshu2=0;

void TIM3_IRQHandler(void)		//	  //TIM3的溢出更新中断响应函数，产生pwm波
{
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 清空TIM3溢出中断响应函数标志位
	if(flag1==0){	
		if(counter1==0){
			GPIO_SetBits(GPIOA,STEP_1);
			counter1++;
		}
		else{
			GPIO_ResetBits(GPIOA,STEP_1);
			counter1++;
			if(counter1>=jishi1){
				counter1=0;
			}
		}
	}
	else if(maichongshu1>0){
		if(counter1==0){
			GPIO_SetBits(GPIOA,STEP_1);
			counter1++;
			maichongshu1--;
		}
		else{
			GPIO_ResetBits(GPIOA,STEP_1);
			counter1++;
			if(counter1>=jishi1){
				counter1=0;
			}
		}		
	}
	else {};
}	



void setPulsePerSecond1(u32 pulse){
	//pulse为每秒脉冲个数
	flag1=0;	
	if(pulse==0){
		disable1();	
	}	
	else{
		jishi1=1000000/(YUZHI+1)/pulse;
		enable1();	
	}
}

void setPulseNumber1(u32 pulse,u32 number){
	//pulse为每秒脉冲个数,number为脉冲数（步数）
	flag1=1;	
	if(pulse==0){
		disable1();	
	}	
	else{
		jishi1=1000000/(YUZHI+1)/pulse;
		maichongshu1=number;
		enable1();	
	}
}

void setSteps1(u32 pulse,u32 number){
	setPulseNumber1(pulse,number);
	while(maichongshu1!=0){
		delay_nms(10);
	}
}

//设置分辨率，值越大一个脉冲转动越小
void setResolution1(u8 resolution){
	switch(resolution){
		case FULL_STEP:
			GPIO_ResetBits(GPIOA,MS1_1|MS2_1|MS3_1);
			break;
		case HALF_STEP:
			GPIO_ResetBits(GPIOA,MS2_1|MS3_1);
			GPIO_SetBits(GPIOA,MS1_1);
			break;
		case QUARTER_STEP:
			GPIO_ResetBits(GPIOA,MS1_1|MS3_1);
			GPIO_SetBits(GPIOA,MS2_1);
			break;
		case EIGHTH_STEP:
			GPIO_ResetBits(GPIOA,MS3_1);
			GPIO_SetBits(GPIOA,MS1_1|MS2_1);
			break;
		case SIXTEENTH_STEP:
			GPIO_SetBits(GPIOA,MS1_1|MS2_1|MS3_1);
			break;
		default:
			GPIO_ResetBits(GPIOA,MS1_1|MS2_1|MS3_1);
			break;
	}
}

void disable1(void){
		GPIO_SetBits(GPIOA,ENABLE_1);
}

void enable1(void){
		GPIO_ResetBits(GPIOA,ENABLE_1);	
}

void setDirection1(u8 dir){
	if(dir){
		GPIO_ResetBits(GPIOA,DIR_1);
	}
	else{
		GPIO_SetBits(GPIOA,DIR_1);
	}
}


void tim3()							  //配置TIM3为基本定时器模式 ，约10us触发一次，触发频率约100kHz
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =YUZHI;		  //配置计数阈值为9，超过时，自动清零，并触发中断
	  TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 时钟预分频值，除以多少
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // 时钟分频倍数
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // 计数方式为向上计数

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 初始化tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除TIM3溢出中断标志
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  使能TIM3的溢出更新中断
    TIM_Cmd(TIM3,ENABLE);					  //		   使能TIM3
}

void nvic()									//配置中断优先级
{	 
    NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  命名一优先级变量

 	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //要用同一个Group
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	溢出更新中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	打断优先级为1，与上一个相同，不希望中断相互打断对方
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	//	响应优先级1，低于上一个，当两个中断同时来时，上一个先执行
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RCC_Configuration(void)				 //使用任何一个外设时，务必开启其相应的时钟
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	  //使能APB2控制外设的时钟，包括GPIOC, 功能复用时钟AFIO等，
                                                                              //其他包括的外设，详见固件库手册等资料
	
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能APB1控制外设的时钟，定时器tim3、4，其他外设详见手册

                
}


void GPIO_Configuration(void)			 //使用某io口输入输出时，请务必对其初始化配置
{
   GPIO_InitTypeDef GPIO_InitStructure;   //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure  
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //配置IO口工作模式为	推挽输出（有较强的输出能力）
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //配置IO口最高的输出速率为50M
	 GPIO_InitStructure.GPIO_Pin = ENABLE_1|MS1_1|MS2_1|MS3_1|STEP_1|DIR_1;	 //配置被选中的管脚，|表示同时被选中
   GPIO_Init(GPIOA, &GPIO_InitStructure);		  //初始化GPIOC的相应IO口为上述配置，用于led输出
   
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//失能STM32 JTAG烧写功能，只能用SWD模式烧写，解放出PA15和PB中部分IO口
}

void Stepmotor_Configuration(void){

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	  //使能APB2控制外设的时钟，包括GPIOC, 功能复用时钟AFIO等，
                                                                              //其他包括的外设，详见固件库手册等资料
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能APB1控制外设的时钟，定时器tim3、4，其他外设详见手册
  
	RCC_Configuration();															   		
  GPIO_Configuration();							
  nvic(); 
	tim3();
	    
}


void delay_nus(u32 n)		//72M时钟下，约延时us
{
  u8 i;
  while(n--)
  {
    i=7;
    while(i--);
  }
}

void delay_nms(u32 n)	  //72M时钟下，约延时ms
{
    while(n--)
      delay_nus(1000);
}


