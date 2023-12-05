#include "stm32f10x.h"
#include "turn.h"
#include "microsonic.h"
#include "Delay.h"
#include "pwm.h"

void turn_init()
{
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 144-1;//为了让每个周期为20ms
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//由后续角度函数来调节寄存器值，进一步改变占空比
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	
	
	TIM_Cmd(TIM5, ENABLE);
	//TIM_CtrlPWMOutputs(TIM5, );这一行加入也没用
}

void set_angle(float angle)
{
	angle=(u16)(50.0*angle/9.0+250.0);//算式倒是对的，但为什么是249，不是250还需看一下？
	TIM_SetCompare1(TIM5,angle);
}

int front_detection()
{
	set_angle(90);
	delay_ms(100);
	return measure_micro();
}
int left_detection()
{
	set_angle(175);//这里与我的理解正好相反，单独测试该模块。
	delay_ms(300);//这个参数也还没看过，需要再确认
	return measure_micro();
}
int right_detection()
{
	set_angle(5);
	delay_ms(300);
	return measure_micro();
}


void microsonic_check()
{
	
		int Q_temp,L_temp,R_temp;
		Q_temp = front_detection();
		if(Q_temp>0 && Q_temp<FRONT_DIS)//0，200这已经是一个比较极限的位置了，我现在给到400，有点像起一个预防作用
		{	
			
			car_brake();
			
			L_temp = left_detection();
			delay_ms(250);		//这个地方的延时是等超声波模块收到回波，这个时候就转向，对测距有很大影响
			R_temp = right_detection();
			delay_ms(250);
			if(L_temp<LR_DTS && R_temp<LR_DTS)//前面全是障碍，就往一边转一定方向,实际上需要达到掉头的作用
			{
				car_spin_left(100);		//实际上是要掉头的意思
				delay_ms(700);
			}
			else if(L_temp>R_temp)
			{
				car_spin_left(100);
				delay_ms(300);
			}
			else
			{
				car_spin_right(100);
				delay_ms(300);
			}
		}	
		else
		{
			car_run(60);//10.25.22:07  yuan
			delay_ms(10);
		}
	
}

