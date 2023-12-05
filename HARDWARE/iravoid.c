#include "stm32f10x.h"
#include "iravoid.h"
#include "Delay.h"
#include "pwm.h"

void IRAvoid_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = IR_LEFT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = IR_RIGHT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

u8 iravoid_left()
{
	u8 temp;
	temp = GPIO_ReadInputDataBit(GPIOA, IR_LEFT);
	return temp;
}

u8 iravoid_right()
{
	u8 temp;
	temp = GPIO_ReadInputDataBit(GPIOB, IR_RIGHT);
	return temp;
}

void iravoid_check()
{
		u8 left,right;
		left = iravoid_left();
		right = iravoid_right();
		if((left == BARRIER_N)&&(right == BARRIER_N))
		{
			car_run(60);
			delay_ms(10);
		}
		else if((left == BARRIER_Y)&&(right == BARRIER_N))
		{
			car_spin_right(100);
			delay_ms(150);
		}
		else if((left == BARRIER_N)&&(right == BARRIER_Y))
		{
			car_spin_left(100);
			delay_ms(150);
		}
		else
		{
			car_back(100);
			delay_ms(50);
			car_spin_right(100);
			delay_ms(50);
		}
}
