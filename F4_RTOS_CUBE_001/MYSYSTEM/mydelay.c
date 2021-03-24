#include "mydelay.h"
#include "mysys.h"
#include "myusart.h"

static u32 fac_us=0;							//���ڸ���ʱ�ӣ�����1us�ӳ٣�����Ҫ��systick����ֵ



			   
//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪAHBʱ��
//SYSCLK:ϵͳʱ��Ƶ��
void delay_init(u8 SYSCLK)
{
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//��ϵͳ��ʱ��������ʱ�Ӷ���Ϊϵͳ����ʱ��
	fac_us=SYSCLK;						
	//���˱�����ֵΪϵͳʱ�ӵ�����������180�����Ǿ���ζ�ţ�ϵͳ��ʱ���ļ���ֵ˥��180��ʱ��͹�ȥ��1u��
	//˥��180*1000��ʱ��͹�ȥ��1ms��������

}								    


//�˺�����С��ȷ��Ϊ 1000/ʱ�ӣ���λ��M��ns  ����120Mʱ�ӣ���ȷ��Ϊ8.33����
//����8.33������Ͷ���һ��8.33ns
void delay_ns(u32 nns)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ
	
	float multi=0;//������Ҫ��Ҫ��������(����ֵ)
	float mintime=8.3333;
	
	multi=nns/mintime;
	ticks=(u32)multi;
	if(multi>ticks)
	{
		ticks++;
	}
	//printf("TICK=%d",ticks);
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;						//ѭ����ȡ���ڵļ�����ֵ
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//û�в������
			else tcnt+=reload-tnow+told;	//���������    
			told=tnow;										//����told���������������������
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}
	}
}
	



//��ʱnus
//nusΪҪ��ʱ��us��.	
//nus:0~190887435(���ֵ��2^32/fac_us@fac_us=22.5)	 
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=nus*fac_us; 							//�����ӳ���Ӧus����Ҫ�ļ���ֵ 
	//printf("TICK=%d",ticks);
	told=SysTick->VAL;        				//��ȡ�ս���ʱϵͳ��ʱ���ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;						//ѭ����ȡ���ڵļ�����ֵ
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//û�в������
			else tcnt+=reload-tnow+told;	//���������    
			told=tnow;										//����told���������������������
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
}

//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}

void delay_s( uint32_t time_s )
{
  for(;time_s>0;time_s--)
    delay_ms(1000);
}


//ϵͳ��ʱ�������ж�,ͬʱ�����ж�װ��ֵ����ռ�����
//�������ʱ�����ж�Ĭ���ǿ����ģ����ж�ʱ������Ϊ1ms�����ң�����жϲ��ܹرգ�����ϵͳ�޷����У�������ô���������ж�ʱ�䣩
//����1��ϵͳʱ��Ƶ�ʣ���λ��M��
//����2���ж�ʱ��������λ��ms,ϵͳ��ʱ����װ��ֵ���ܳ���24λ,�����ӳ�Ϊ��16777215/(sysclkM)/1000��
//u16 sysclkM,u16 nms
void mysystickIT(u16 sysclkM,u16 nms)
{
	SysTick_Config(nms*sysclkM*1000);
}





