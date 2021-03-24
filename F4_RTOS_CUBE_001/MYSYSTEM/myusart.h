#ifndef _USART_H
#define _USART_H
#include "mysys.h"
#include "stdio.h"	
#include "main.h"

#define USART_REC_LEN  			400  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define IF_USE_DMARX		1			//�Ƿ�ʹ��DMA���մ�������
#define IF_USE_DMATX		1			//�Ƿ�ʹ��DMAģʽ���ʹ�������
			
extern u8 USART_RXDMA_BUF[USART_REC_LEN];	//DMA���ջ�����
extern u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u32 USART_RX_NUM;       //���յ��ֽ���Ŀ��¼
extern u32 USART_RX_OK;//��ǽ������

extern UART_HandleTypeDef huart1;

#if IF_USE_DMATX==1
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
#endif

int fputc(int ch, FILE *f);
/*
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
*/
u8 MY_USARTDMA_TX(u8* txbuf,u16 txnum);

#endif
