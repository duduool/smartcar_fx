#include "pit.h"

//=================================================================================================
// 初始化PIT模块
//=================================================================================================
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct)
{	 
	uint32_t time = 0;
	//计算分频数
	GetCPUInfo();
	time = (PIT_InitStruct->PIT_Interval)*(CPUInfo.BusClock/1000);
	//使能PIT时钟
	SIM->SCGC6|=SIM_SCGC6_PIT_MASK;
	//使能模块
	PIT->MCR&=~PIT_MCR_MDIS_MASK;
	//开始计时
	PIT->MCR|=PIT_MCR_FRZ_MASK;
	
	PIT->CHANNEL[PIT_InitStruct->PITx].LDVAL = (uint32_t)time;//设置延时时间
	PIT->CHANNEL[PIT_InitStruct->PITx].TFLG =0;//|= PIT_TFLG_TIF_MASK;//清除标志位
	PIT->CHANNEL[PIT_InitStruct->PITx].TCTRL|= (PIT_TCTRL_TEN_MASK);//开启定时器及中断
}


//=================================================================================================
// PIT 获得预设定时器值大小
//=================================================================================================
uint32_t PIT_GetLoadValue(uint8_t PITx)
{
	return 	PIT->CHANNEL[PITx].LDVAL;
}


//=================================================================================================
// PIT 获得当前运行定时器值大小
//=================================================================================================
uint32_t PIT_GetCurrentValue(uint8_t PITx)
{	
	return PIT->CHANNEL[PITx].CVAL;
}


//=================================================================================================
// PIT 设置预设值大小
//=================================================================================================
void PIT_SetLoadValue(uint8_t PITx, uint32_t Value)
{
	PIT->CHANNEL[PITx].LDVAL = Value;
}


//=================================================================================================
// PIT 开始工作
//=================================================================================================
void PIT_Start(uint8_t PITx)
{
	PIT->CHANNEL[PITx].TCTRL |= PIT_TCTRL_TEN_MASK;
}


//=================================================================================================
// PIT 暂停
//=================================================================================================
void PIT_Stop(uint8_t PITx)
{
	PIT->CHANNEL[PITx].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}


//=================================================================================================
// PIT 中断配置
//=================================================================================================
void PIT_ITConfig(uint8_t PITx, uint16_t PIT_IT, FunctionalState NewState)
{
	if(PIT_IT == PIT_IT_TIF)
	{
		(ENABLE == NewState)?(PIT->CHANNEL[PITx].TCTRL |= PIT_TCTRL_TIE_MASK):(PIT->CHANNEL[PITx].TCTRL &= ~PIT_TCTRL_TIE_MASK);
	}
}


//=================================================================================================
//PIT 获得中断标志
//=================================================================================================
ITStatus PIT_GetITStatus(uint8_t PITx, uint16_t PIT_IT)
{
	if(PIT_IT == PIT_IT_TIF)
	{
		if(PIT->CHANNEL[PITx].TFLG & PIT_TFLG_TIF_MASK)
		{
			return SET;
		}
		else
		{
			return RESET;
		}
	}
	return RESET;
}


//=================================================================================================
// PIT 清除中断标志
//=================================================================================================
void PIT_ClearITPendingBit(uint8_t PITx,uint16_t PIT_IT)
{
	PIT->CHANNEL[PITx].TFLG |= PIT_TFLG_TIF_MASK;
}
