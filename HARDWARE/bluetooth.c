#include "stm32f10x.h"
#include "bluetooth.h"

__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];//���д�����Ȼ֪���ǰ��ֽڶ��룬������ô��ȴ�ǲ�֪����C����
char USART2_RX_BUF[USART2_MAX_RECV_LEN]; 
void usart_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
}

			//�������ö�ʱ���ж�ʵ�ʲ���GPIO�������뿴����ȽϺ����벶���Ǻ��ⲿ�йص���
void tim3_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	//TIM_Cmd(TIM3, ENABLE);��Ҫ���ڿ���Ҫ�ȵ����ڿ�ʼ�����ݵ�ʱ���ٿ�
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

u16	USART2_RX_STA = 0;
u8 reclen = 0;
void USART2_IRQHandler()
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		if(USART2_RX_STA == 0)//��ʼ�յ���һ������
		{
			TIM_Cmd(TIM3, ENABLE);
		}
		res = USART_ReceiveData(USART2);
		TIM_SetCounter(TIM3, 0);//���д���Ӧ���ڽ��յ�һ��֮����Ϊ��ʱ�������ÿ������֮��ļ��
		USART2_RX_BUF[USART2_RX_STA] = res;
		USART2_RX_STA++;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}

void TIM3_IRQHandler()
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update))
	{
		USART2_RX_STA |= 0x8000;//�õ�16λ��һ����־������ɡ���ʲôʱ���λ��0��10.24��˵�������ڵ��뷨����������0��
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//���㲻Ҫ��������жϱ�־λ
		TIM_Cmd(TIM3, DISABLE);//������֪��Ϊʲô��ʱ��Ҫ�ر��ˣ�û���źŵ�ʱ���˾���Ӱ�������Ҫ�ṹ
	}

}

