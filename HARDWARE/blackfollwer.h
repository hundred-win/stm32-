#ifndef __BLACK_H
#define __BLACK_H

//u8 black_follow_left(void);
//u8 black_follow_right(void);
void black_follow_all(void);
void blackfollower_init(void);//����������ʼ��Ҳ������
extern u8 reclen;
#define		BLACK	1
#define		WHITE	0

#define		BFL_PIN		GPIO_Pin_0		//�����Ҫ��ȥ��ϸ��һ�飻
#define 	BFR_PIN		GPIO_Pin_7
#define		BFL_GPIO	GPIOB
#define 	BFR_GPIO	GPIOA

#define		RUN		4
#define 	LEFT	5
#define		RIGHT	6
#define		STOP	7

#endif

