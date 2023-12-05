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
	TIM_TimeBaseInitStructure.TIM_Prescaler = 144-1;//Ϊ����ÿ������Ϊ20ms
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//�ɺ����ǶȺ��������ڼĴ���ֵ����һ���ı�ռ�ձ�
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	
	
	TIM_Cmd(TIM5, ENABLE);
	//TIM_CtrlPWMOutputs(TIM5, );��һ�м���Ҳû��
}

void set_angle(float angle)
{
	angle=(u16)(50.0*angle/9.0+250.0);//��ʽ���ǶԵģ���Ϊʲô��249������250���迴һ�£�
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
	set_angle(175);//�������ҵ���������෴���������Ը�ģ�顣
	delay_ms(300);//�������Ҳ��û��������Ҫ��ȷ��
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
		if(Q_temp>0 && Q_temp<FRONT_DIS)//0��200���Ѿ���һ���Ƚϼ��޵�λ���ˣ������ڸ���400���е�����һ��Ԥ������
		{	
			
			car_brake();
			
			L_temp = left_detection();
			delay_ms(250);		//����ط�����ʱ�ǵȳ�����ģ���յ��ز������ʱ���ת�򣬶Բ���кܴ�Ӱ��
			R_temp = right_detection();
			delay_ms(250);
			if(L_temp<LR_DTS && R_temp<LR_DTS)//ǰ��ȫ���ϰ�������һ��תһ������,ʵ������Ҫ�ﵽ��ͷ������
			{
				car_spin_left(100);		//ʵ������Ҫ��ͷ����˼
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

