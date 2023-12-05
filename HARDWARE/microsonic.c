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
	
	
	EXTI_InitTypeDef EXTI_InitStructure;//����ģ����жϾ����ⲿ�ж�
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
	
	//�����Ƕ�ʱ��2��ʼ��
	
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
		UltrasonicWave_Distance=TIM_GetCounter(TIM6)*34/20.0;//����������ʽ�һ�û��ȥʵ�ʲ������
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int measure_micro(void)
{
	int u_temp;
	GPIO_SetBits(GPIO_MICRO, PIN_TRIG); 		  //��>10US�ĸߵ�ƽ�TRIG_PORT,TRIG_PIN��������define����?
	delay_us(20);		                      //��ʱ20US,��Ϊ�����÷����źŷ������
	GPIO_ResetBits(GPIO_MICRO, PIN_TRIG);
	u_temp = UltrasonicWave_Distance*10;		//����ط�������ʮ��Ҳ�����
	return u_temp;
}




