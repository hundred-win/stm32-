#ifndef __pwm_h
#define __pwm_h

void pwm_init(void);
void set_speed_left(u8 speed);
void set_speed_right(u8 speed);
//void set_direction(void);	//��Щ�����ڲ㺯��

void car_run(u8 speed); 	//������װ����
void car_back(u8 speed);
void car_left(u8 speed);
void car_right(u8 speed);
void car_spin_left(u8 speed);
void car_spin_right(u8 speed);
void car_brake(void);

//PB7������PB8��PWM�� PA4�ҵ����PB9��PWM����ʱ��ѡ��TIM4��ͨ��������
#define MOTOR_LEFT	GPIO_Pin_7		//��һ���ǳ�ţ�ƵĴ�������Ĺٷ�����������ҵ�������Ƿ���	
#define MOTOR_RIGHT	GPIO_Pin_4
#define PWM_LEFT	GPIO_Pin_8
#define PWM_RIGHT	GPIO_Pin_9

#define SET_ANTICW_LEFT		 	GPIO_SetBits(GPIOB, MOTOR_LEFT)			//һ�����ķ���
#define SET_CW_LEFT		GPIO_ResetBits(GPIOB, MOTOR_LEFT)
#define SET_ANTICW_RIGHT		GPIO_SetBits(GPIOA, MOTOR_RIGHT)
#define SET_CW_RIGHT	GPIO_ResetBits(GPIOA, MOTOR_RIGHT)

#endif
