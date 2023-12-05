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
	
										//�����������ʼ��
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
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;	//��һ��ʵ����˵������ʱ����֪�������õ��Լ���Ҫ��ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM4, ENABLE);
	
}


//����ΰ��ʤ������תʵ�����ǵ���100-���ٶȣ�����ת�����

void set_speed_left(u8 speed)
{
	u8 spwm = 100 - speed;
	TIM_SetCompare4(TIM4, spwm);//��ʵӦ�ü�һ������������
}

void set_speed_right(u8 speed)
{
	u8 spwm = 100 - speed;
	TIM_SetCompare3(TIM4, spwm);
}

//ʵ������װ

void car_run(u8 speed)
{
	SET_CW_LEFT;
	SET_CW_RIGHT;//���߷��򶨺��ˣ����ھ�ȷ���ٶ�
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
	SET_CW_LEFT;//10.12�˵�18���ҰѴ����Ϊ��������ȷ�����Ͼŵ�����������֪��Ϊ�δ���������⣬����Ϊ����ת����Ч��ƽ��ͬ������ռ�ձ�Ҳ�����񡱲�ͬ
	
	set_speed_right(speed);
}

void car_spin_right(u8 speed)
{
	SET_ANTICW_LEFT;
	SET_CW_RIGHT;			//��������ܼ򵥣����ǽӷ��˵ģ�������������תǡ���෴
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




