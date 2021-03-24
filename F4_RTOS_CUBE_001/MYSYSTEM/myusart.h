#ifndef _USART_H
#define _USART_H
#include "mysys.h"
#include "stdio.h"	
#include "main.h"

#define USART_REC_LEN  			400  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define IF_USE_DMARX		1			//是否使用DMA接收串口数据
#define IF_USE_DMATX		1			//是否使用DMA模式发送串口数据
			
extern u8 USART_RXDMA_BUF[USART_REC_LEN];	//DMA接收缓冲区
extern u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u32 USART_RX_NUM;       //接收的字节数目记录
extern u32 USART_RX_OK;//标记接收完成

extern UART_HandleTypeDef huart1;

#if IF_USE_DMATX==1
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
#endif

int fputc(int ch, FILE *f);
/*
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
*/
u8 MY_USARTDMA_TX(u8* txbuf,u16 txnum);

#endif
