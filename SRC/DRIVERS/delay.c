#include "delay.h"

static uint8_t  m=0;//us延时参数
static uint32_t n=0;//ms延时参数

//================================初始化定时器===============================//
//入口参数：无
//返回值：无
//实现延时前的定时器初始化
//============================================================================//
void DelayInit(void)
{
	SysTick->CTRL|=0x04;					//设置时钟源为内核时钟
	GetCPUInfo();  							//计算系统时钟
	m=CPUInfo.CoreClock/1000000;
	n=(uint32_t)m*1000;
}

//================================微秒级延时==================================//
//入口参数：延时数
//返回值：无
//实现微秒级延时
//============================================================================//
void DelayUs(uint32_t us)
{ 
    uint32_t temp;
    SysTick->LOAD=us*m; 					//时间加载
    SysTick->VAL=0x00;   					//清空计数器
    SysTick->CTRL|=0x01	;					//开始倒数   
	do {
		temp=SysTick->CTRL;
	} while(temp&0x01&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL&=~0x01;      				//关闭计数器
}

//================================微秒级延时==================================//
//入口参数：毫秒级延时数
//返回值：无
//实现毫秒级延时
//============================================================================//
void DelayMs(uint32_t ms)    
{
	uint32_t temp;
	uint16_t i;
	for(i=0;i<ms;i++)      			//延时 MS 
	{
		SysTick->LOAD=n;  				//时间加载1ms
		SysTick->VAL=0x00;    		//清空计数器
		SysTick->CTRL|=0x01;	 	 //开始倒数   
		do {
            temp=SysTick->CTRL;
		} while(temp&0x01&&!(temp&(1<<16)));  //等待时间到达   
		SysTick->CTRL&=~0x01;             	//关闭计数器
	}
}

void SamplingDelay(void) {
    volatile uint8_t i ;
    for(i = 0; i < 1; i++) {
        __NOP();
        __NOP();
    }

}
