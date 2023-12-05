#include "stm32f10x.h"
#include "microsonic.h"
#include "Delay.h"

float UltrasonicWave_Distance;
void microsonic_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = PIN_TRIG;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MICRO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = PIN_ECHO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_MICRO, &GPIO_InitStructure);
	
	
	EXTI_InitTypeDef EXTI_InitStructure;//超声模块的中断就是外部中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	//现在是定时器2初始化
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 5000-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);
}

void EXTI1_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line1))
	{
		TIM_Cmd(TIM6, ENABLE);
		TIM_SetCounter(TIM6, 0);
		while(GPIO_ReadInputDataBit(GPIO_MICRO, PIN_ECHO));
		TIM_Cmd(TIM6, DISABLE);
		UltrasonicWave_Distance=TIM_GetCounter(TIM6)*34/20.0;//这个距离的算式我还没有去实际测试理解
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int measure_micro(void)
{
	int u_temp;
	GPIO_SetBits(GPIO_MICRO, PIN_TRIG); 		  //送>10US的高电平RIG_PORT,TRIG_PIN这两个在define中有?
	delay_us(20);		                      //延时20US,是为了能让发射信号发射完成
	GPIO_ResetBits(GPIO_MICRO, PIN_TRIG);
	u_temp = UltrasonicWave_Distance*10;		//这个地方的扩大十倍也很奇怪
	return u_temp;
}




