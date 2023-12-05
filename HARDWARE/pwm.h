#ifndef __pwm_h
#define __pwm_h

void pwm_init(void);
void set_speed_left(u8 speed);
void set_speed_right(u8 speed);
//void set_direction(void);	//这些都是内层函数

void car_run(u8 speed); 	//最外层封装函数
void car_back(u8 speed);
void car_left(u8 speed);
void car_right(u8 speed);
void car_spin_left(u8 speed);
void car_spin_right(u8 speed);
void car_brake(void);

//PB7左电机，PB8左PWM， PA4右电机，PB9右PWM，定时器选择TIM4，通道三和四
#define MOTOR_LEFT	GPIO_Pin_7		//有一个非常牛逼的错误，他妈的官方设计连线左右电机引脚是反的	
#define MOTOR_RIGHT	GPIO_Pin_4
#define PWM_LEFT	GPIO_Pin_8
#define PWM_RIGHT	GPIO_Pin_9

#define SET_ANTICW_LEFT		 	GPIO_SetBits(GPIOB, MOTOR_LEFT)			//一侧电机的方向
#define SET_CW_LEFT		GPIO_ResetBits(GPIOB, MOTOR_LEFT)
#define SET_ANTICW_RIGHT		GPIO_SetBits(GPIOA, MOTOR_RIGHT)
#define SET_CW_RIGHT	GPIO_ResetBits(GPIOA, MOTOR_RIGHT)

#endif
