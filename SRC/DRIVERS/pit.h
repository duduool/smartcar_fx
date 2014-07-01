
#ifndef __PIT_H__
#define __PIT_H__

#include "sys.h"

#define PIT0   (uint8_t)(0)
#define PIT1   (uint8_t)(1)

#define PIT_IT_TIF     (uint16_t)(0)

//PIT 初始化
typedef struct
{
	uint8_t PITx;             //PIT通道
	uint32_t PIT_Interval;    //PIT定时时间 单位MS
}PIT_InitTypeDef;

//接口函数
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct);
uint32_t PIT_GetLoadValue(uint8_t PITx);
uint32_t PIT_GetCurrentValue(uint8_t PITx);
void PIT_SetLoadValue(uint8_t PITx, uint32_t Value);
void PIT_Start(uint8_t PITx);
void PIT_Stop(uint8_t PITx);
void PIT_ITConfig(uint8_t PITx, uint16_t PIT_IT, FunctionalState NewState);
ITStatus PIT_GetITStatus(uint8_t PITx, uint16_t PIT_IT);
void PIT_ClearITPendingBit(uint8_t PITx,uint16_t PIT_IT);


#endif
