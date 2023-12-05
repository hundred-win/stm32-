#include "stm32f10x.h"
#include "blackfollwer.h"
#include "pwm.h"
#include "bluetooth.h"

//很大的一个失误，不如例程，是没有考虑到如果没变化实际上不需要改变（回去可以测试下对速度的影响）
int command = 7;
int last_command = 7;

u8	black_follow_left(void)
{
	if((GPIO_ReadInputDataBit(BFL_GPIO, BFL_PIN))==1)
		return BLACK;
	else return WHITE;
}

u8	black_follow_right(void)
{
	if((GPIO_ReadInputDataBit(BFR_GPIO, BFR_PIN))==1)
		return BLACK;
	else return WHITE;
}
void blackfollower_init()
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//看引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = BFL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BFL_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = BFR_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BFR_GPIO, &GPIO_InitStructure);
}


void black_follow_all(void)
{
	if(((black_follow_left()==WHITE))&&((black_follow_right())==WHITE))
		car_run(40);
	else if(((black_follow_left()==WHITE))&&((black_follow_right())==BLACK))
		car_spin_right(75);
	else if(((black_follow_left()==BLACK))&&((black_follow_right())==WHITE))
		car_spin_left(75);
	else
		car_spin_left(75);
	
	
}





