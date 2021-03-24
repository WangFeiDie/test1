#include "mydelay.h"
#include "mysys.h"
#include "myusart.h"

static u32 fac_us=0;							//用于根据时钟，计算1us延迟，所需要的systick计数值



			   
//初始化延迟函数
//SYSTICK的时钟固定为AHB时钟
//SYSCLK:系统时钟频率
void delay_init(u8 SYSCLK)
{
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//将系统定时器的驱动时钟定义为系统运行时钟
	fac_us=SYSCLK;						
	//将此变量赋值为系统时钟的兆数（例如180），那就意味着，系统定时器的计数值衰减180，时间就过去了1u秒
	//衰减180*1000，时间就过去了1ms。。。。

}								    


//此函数最小精确度为 1000/时钟（单位：M）ns  对于120M时钟，精确度为8.33纳秒
//除以8.33，有余就多算一个8.33ns
void delay_ns(u32 nns)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值
	
	float multi=0;//计算需要需要计数多少(浮点值)
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
		tnow=SysTick->VAL;						//循环读取现在的计数器值
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//没有产生溢出
			else tcnt+=reload-tnow+told;	//产生了溢出    
			told=tnow;										//更新told，否则上面的语句就有问题
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}
	}
}
	



//延时nus
//nus为要延时的us数.	
//nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)	 
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 							//换算延迟相应us所需要的计数值 
	//printf("TICK=%d",ticks);
	told=SysTick->VAL;        				//读取刚进入时系统定时器的计数器值
	while(1)
	{
		tnow=SysTick->VAL;						//循环读取现在的计数器值
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//没有产生溢出
			else tcnt+=reload-tnow+told;	//产生了溢出    
			told=tnow;										//更新told，否则上面的语句就有问题
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}

//延时nms
//nms:要延时的ms数
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


//系统定时器配置中断,同时配置中断装载值，清空计数器
//（这个定时器的中断默认是开启的，且中断时间配置为1ms，并且，这个中断不能关闭，否则系统无法运行，但是我么可以配置中断时间）
//参数1：系统时钟频率（单位：M）
//参数2：中断时间间隔（单位：ms,系统定时器的装载值不能超过24位,最大的延迟为：16777215/(sysclkM)/1000）
//u16 sysclkM,u16 nms
void mysystickIT(u16 sysclkM,u16 nms)
{
	SysTick_Config(nms*sysclkM*1000);
}





