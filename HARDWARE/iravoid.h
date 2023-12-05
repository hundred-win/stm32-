#ifndef __IRAVOID_H
#define __IRAVOID_H

void IRAvoid_Init(void);
u8 iravoid_left(void);
u8 iravoid_right(void);

#define IR_LEFT		GPIO_Pin_8//GPIOA
#define IR_RIGHT	GPIO_Pin_1//GPIOB    高电平表示无障碍
#define LED_LEFT	GPIO_Pin_0
#define LED_RIGHT	GPIO_Pin_0
#define BARRIER_Y	0
#define BARRIER_N	1

void iravoid_check(void);

#endif
