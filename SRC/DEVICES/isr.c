#include "isr.h"
#include "adc.h"
#include "ccd.h"
#include "pit.h"
#include "led.h"
#include "pwm.h"
#include "pid.h"
#include "cntl.h"
#include "delay.h"

//======================================================================
//PIT中断配置函数
//入口：PIT通道：PITx  ，时间间隔：timePIT
//返回：无
//======================================================================
void Config_PIT(uint8_t PITx , uint32_t timePIT)
{
	PIT_InitTypeDef PITInit;                  //定义pit配置结构
	PITInit.PITx=PITx;                        //pit通道
	PITInit.PIT_Interval=timePIT;             //pit时间间隔
	PIT_Init(&PITInit);                       //传送设置
	PIT_ITConfig( PITx,PIT_IT_TIF,ENABLE );   //配置中断
    /* 以下部分在初始化后必须运行，若有其他中断可在主函数添加 */
	NVIC_EnableIRQ(PIT_IRQn);                 //开启优先管理
	PIT_Start(PITx);                          //开启PIT
}

/* 控制小灯 */
static void TwinkleLED_Callback(void)
{
    ITStatus state;
 	state = PIT_GetITStatus(PIT0, PIT_IT_TIF);
    
    if(state == SET) {
		TwinkleLed(PTA,14);
 	}
}

//======================================================================
//定时中断事件函数
//入口：无
//返回：无
//======================================================================
void PIT_IRQHandler(void)
{
    /* 小灯常亮表明，进入中断 */
    TwinkleLED_Callback();
    PIT_ClearITPendingBit(PIT0, PIT_IT_TIF);
}

//======================================================================
//低功耗定时器中断事件函数
//入口：无
//返回：无
//======================================================================
void LPTimer_IRQHandler(void)
{
    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
}
