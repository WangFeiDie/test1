#include "myusart.h"
#include "mydelay.h"
#include "string.h"

 
//printf֧�ֺ���
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 10);
	return ch;
}




#if IF_USE_DMATX==1
//��HAL���DMA���ͺ������з�װ
//u8 MY_USARTDMA_TX(u8* txbuf,u16 txnum)
//{
//	//���DMA���ھ���״̬������з��ͣ����򲻷���
//	if(HAL_DMA_GetState(&hdma_usart1_tx)==HAL_DMA_STATE_READY)
//	{
//		HAL_UART_Transmit_DMA(&huart1, txbuf, txnum);
//		return 1;
//	}
//	else
//	{
//		//����ʧ�ܣ�����0
//		return 0;
//	}
//	
//}


#endif





/*****************************************************************************
 * ָ�����\r\n
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
				//�ٿ���ָ��
				case 0x01:
					switch(recbuf[5])
					{
						default:
							printf("ָ�����\r\n");
						break;
					}
			}
		}
		else
		{
			printf("ָ�����\r\n");
		}
	}
	else
	{
		printf("ָ�����\r\n");
	}
}





#if EN_USART1_RX   //���ʹ���˽���


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RXDMA_BUF[USART_REC_LEN];	//DMA���ջ�����
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u32 USART_RX_NUM=0;       //���յ��ֽ���Ŀ��¼	
u32 USART_RX_OK001=0;//��ǽ�����ɲ���
u32 USART_RX_OK=0;//��ǽ������
extern void COMMOND_DEAL(u8* recbuf);


#if	IF_USE_DMARX==1
//extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

//ʹ��DMA���մ�������ʱ����Ҫ����������ͷ���������仰��L4�İ����ڲ��Ե�ʱ��һ��ʼ�����̾ͻ����һ���жϣ���F4��û�У�����Ӧ����Ӳ������
//__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
//	HAL_UART_Receive_DMA(&huart1, USART_RXDMA_BUF, USART_REC_LEN);
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
	
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)==1)//�жϽ��տ����ж��Ƿ���λ
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);//�ֶ����IDLE��־
		
		//�رս��ղ�����ͬʱ�ر�DMAͨ�����������ִ�д˺�������ôDMA���ᱻ���ã�DMA����ָ�뽫������������
		HAL_UART_AbortReceive(&huart1);
		USART_RX_NUM=USART_REC_LEN-__HAL_DMA_GET_COUNTER(&hdma_usart1_rx);//��ȡ�����ݳ���
		memcpy(USART_RX_BUF,USART_RXDMA_BUF,USART_RX_NUM);//�������ݣ�������Բ��ÿ����ķ�����Ҳ���Բ��ø����������ķ���
		USART_RX_OK=1;//��ǽ��������
		
		

		//ָ�����
		COMMOND_DEAL(USART_RX_BUF);
		
		//MY_USARTDMA_TX(USART_RX_BUF,USART_RX_NUM);
		USART_RX_OK=0;
		USART_RX_NUM=0;
		
		
		
		HAL_UART_Receive_DMA(&huart1, USART_RXDMA_BUF, USART_REC_LEN);//����DMA����
	}
}


#else
//����1�жϷ������ ��ʹ��ʱ��MX���ɵĺ���������ͻ����Ҫ��stm32f4xx_it.c�еĺ���ע�͵�
//����������ͷ���� __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
void USART1_IRQHandler(void)                	
{
	u8 Res=0;
	HAL_UART_Receive(&huart1, (uint8_t *)&USART_RX_BUF[USART_RX_NUM], 1, 1000);
	Res=USART_RX_BUF[USART_RX_NUM];
	USART_RX_NUM++;
	if((USART_RX_NUM)>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	
	
	if(USART_RX_OK==0)//����δ���
	{
		if(USART_RX_OK001==1)//�Ѿ����յ���0x0d��������ж�
		{
			if(Res!=0x0a)//�ж����ڽ��ܵ���������Ƿ�Ϊ�س��ڶ����ַ�
			{
				USART_RX_STA=(USART_RX_STA&0XBFFF);//��������Ļس�����˵���������е�
				USART_RX_OK001=0;
			}
			else//�ǻس��ĵڶ��ַ�����ǵ�16λ����ʾ�Ѿ���ɽ���
			{
				USART_RX_OK=1;	//���������	
				USART_RX_OK001=0;				
			}
		}
		else //��û�յ�0X0D
		{
			if(Res==0x0d)//�ж����ڽ��ܵ���������Ƿ�Ϊ�س��ĵ�һ���ַ��������ǵ�15λ
			{
				USART_RX_OK001=1;
			}
		}
	}

	HAL_UART_IRQHandler(&huart1);	
}

#endif


 

#endif	






