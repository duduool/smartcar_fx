
#ifndef __DELAY_H__
#define __DELAY_H__


#include "sys.h"

void DelayInit(void); //延时定时器初始化
void DelayUs(uint32_t us);      //延时us
void DelayMs(uint32_t ms);      //延时ms
void SamplingDelay(void);
		 
#endif
