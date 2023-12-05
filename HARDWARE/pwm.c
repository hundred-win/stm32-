#include "stm32f10x.h"
#include "pwm.h"

void pwm_init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
										//把两个电机初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOTOR_LEFT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOTOR_RIGHT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = PWM_LEFT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = PWM_RIGHT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	//TIM_TimeBaseInitStructure.TIM_RepetitionCounter = ;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;	//这一行实际有说法，暂时还不知道如何想得到自己想要的值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM4, ENABLE);
	
}


//根据伟大胜利，正转实际上是等于100-其速度，而反转是相等

void set_speed_left(u8 speed)
{
	u8 spwm = 100 - speed;
	TIM_SetCompare4(TIM4, spwm);//其实应该加一个界限来保险
}

void set_speed_right(u8 speed)
{
	u8 spwm = 100 - speed;
	TIM_SetCompare3(TIM4, spwm);
}

//实现外层封装

void car_run(u8 speed)
{
	SET_CW_LEFT;
	SET_CW_RIGHT;//两边方向定好了，现在就确定速度
	set_speed_left(speed);
	set_speed_right(speed);
}

void car_back(u8 speed)
{
	SET_ANTICW_LEFT;
	SET_ANTICW_RIGHT;
	u8 temp = 100 - speed;
	set_speed_left(temp);
	set_speed_right(temp);
}

void car_left(u8 speed)
{
	SET_CW_LEFT;
	SET_CW_RIGHT;
	set_speed_left(0);
	set_speed_right(speed);
}
void car_right(u8 speed)
{
	SET_CW_LEFT;
	SET_CW_RIGHT;
	set_speed_left(speed);
	set_speed_right(0);
}
void car_spin_left(u8 speed)
{
	u8 temp = 100 - speed;
	SET_ANTICW_RIGHT;
	set_speed_left(temp);
	SET_CW_LEFT;//10.12八点18，我把代码改为我理解的正确，晚上九点零三，我已知道为何代码会有问题，是因为正反转的有效电平不同，导致占空比也“好像”不同
	
	set_speed_right(speed);
}

void car_spin_right(u8 speed)
{
	SET_ANTICW_LEFT;
	SET_CW_RIGHT;			//最后的问题很简单，线是接反了的，所以左右正反转恰好相反
	u8 temp = 100 - speed;
	set_speed_left(speed);
	set_speed_right(temp);
}
void car_brake()
{
	SET_CW_LEFT;
	SET_CW_RIGHT;
	set_speed_left(0);
	set_speed_right(0);
}




