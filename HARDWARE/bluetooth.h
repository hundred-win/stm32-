#ifndef __BLUE_H
#define __BLUE_H

#define USART2_MAX_RECV_LEN		200
#define USART2_MAX_SEND_LEN		200

extern char  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART2_RX_STA;   						//���λΪ�յ����һ���ı�־������ȫ���кö�����ַ�����

void usart_init(void);
void tim3_init(void);

#endif
