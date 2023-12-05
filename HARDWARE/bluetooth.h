#ifndef __BLUE_H
#define __BLUE_H

#define USART2_MAX_RECV_LEN		200
#define USART2_MAX_SEND_LEN		200

extern char  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART2_RX_STA;   						//最高位为收到完成一个的标志，后面全是有好多个单字符数据

void usart_init(void);
void tim3_init(void);

#endif
