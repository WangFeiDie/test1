#include "myusart.h"
#include "mydelay.h"
#include "string.h"

 
//printf支持函数
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 10);
	return ch;
}




#if IF_USE_DMATX==1
//将HAL库的DMA发送函数进行封装
//u8 MY_USARTDMA_TX(u8* txbuf,u16 txnum)
//{
//	//如果DMA处于就绪状态，则进行发送，否则不发送
//	if(HAL_DMA_GetState(&hdma_usart1_tx)==HAL_DMA_STATE_READY)
//	{
//		HAL_UART_Transmit_DMA(&huart1, txbuf, txnum);
//		return 1;
//	}
//	else
//	{
//		//发送失败，返回0
//		return 0;
//	}
//	
//}


#endif





/*****************************************************************************
 * 指令操作\r\n
 ****************************************************************************/
void COMMOND_DEAL(u8* recbuf)
{
	u16 tempvalue=0;
	
	if(recbuf[0]==0x12 && recbuf[1]==0x34)
	{
		if(recbuf[9]==0xab && recbuf[10]==0xcd)
		{
			switch(recbuf[4])
			{
				//操控型指令
				case 0x01:
					switch(recbuf[5])
					{
						default:
							printf("指令错误\r\n");
						break;
					}
			}
		}
		else
		{
			printf("指令错误\r\n");
		}
	}
	else
	{
		printf("指令错误\r\n");
	}
}





#if EN_USART1_RX   //如果使能了接收


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RXDMA_BUF[USART_REC_LEN];	//DMA接收缓冲区
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u32 USART_RX_NUM=0;       //接收的字节数目记录	
u32 USART_RX_OK001=0;//标记接收完成部分
u32 USART_RX_OK=0;//标记接收完成
extern void COMMOND_DEAL(u8* recbuf);


#if	IF_USE_DMARX==1
//extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

//使用DMA接收串口数据时，需要在主函数开头调用这两句话，L4的板子在测试的时候，一初始化立刻就会进入一次中断，但F4的没有，所以应该是硬件问题
//__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
//	HAL_UART_Receive_DMA(&huart1, USART_RXDMA_BUF, USART_REC_LEN);
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
	
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)==1)//判断接收空闲中断是否置位
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);//手动清除IDLE标志
		
		//关闭接收操作，同时关闭DMA通道流，如果不执行此函数，那么DMA不会被重置，DMA接受指针将持续持续增加
		HAL_UART_AbortReceive(&huart1);
		USART_RX_NUM=USART_REC_LEN-__HAL_DMA_GET_COUNTER(&hdma_usart1_rx);//获取的数据长度
		memcpy(USART_RX_BUF,USART_RXDMA_BUF,USART_RX_NUM);//拷贝数据，这里可以采用拷贝的方法，也可以采用更换缓冲区的方法
		USART_RX_OK=1;//标记接收完成了
		
		

		//指令处理函数
		COMMOND_DEAL(USART_RX_BUF);
		
		//MY_USARTDMA_TX(USART_RX_BUF,USART_RX_NUM);
		USART_RX_OK=0;
		USART_RX_NUM=0;
		
		
		
		HAL_UART_Receive_DMA(&huart1, USART_RXDMA_BUF, USART_REC_LEN);//重启DMA缓冲
	}
}


#else
//串口1中断服务程序 ，使用时与MX生成的函数产生冲突，需要将stm32f4xx_it.c中的函数注释掉
//在主函数开头调用 __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
void USART1_IRQHandler(void)                	
{
	u8 Res=0;
	HAL_UART_Receive(&huart1, (uint8_t *)&USART_RX_BUF[USART_RX_NUM], 1, 1000);
	Res=USART_RX_BUF[USART_RX_NUM];
	USART_RX_NUM++;
	if((USART_RX_NUM)>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	
	
	if(USART_RX_OK==0)//接收未完成
	{
		if(USART_RX_OK001==1)//已经接收到了0x0d，则进入判断
		{
			if(Res!=0x0a)//判断现在接受的这个数据是否为回车第二个字符
			{
				USART_RX_STA=(USART_RX_STA&0XBFFF);//不是连贯的回车符，说明是数据中的
				USART_RX_OK001=0;
			}
			else//是回车的第二字符，标记第16位，表示已经完成接收
			{
				USART_RX_OK=1;	//接收完成了	
				USART_RX_OK001=0;				
			}
		}
		else //还没收到0X0D
		{
			if(Res==0x0d)//判断现在接受的这个数据是否为回车的第一个字符，是则标记第15位
			{
				USART_RX_OK001=1;
			}
		}
	}

	HAL_UART_IRQHandler(&huart1);	
}

#endif


 

#endif	






