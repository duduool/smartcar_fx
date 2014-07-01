
	 
#ifndef __SYS_H__
#define __SYS_H__

#include "MKL25Z4.h"

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {FALSE = 0, TRUE = !FALSE} ErrorState;

#ifndef NULL
	#define NULL 0
#endif


#define  LZVERSION                 (1.0)            

typedef struct
{
	uint32_t m_ModuleIndex:3;
	uint32_t m_PortIndex:3;
	uint32_t m_MuxIndex:3;
	uint32_t m_PinBaseIndex:5;
	uint32_t m_PinCntIndex:3;
	uint32_t m_ChlIndex:5;
	uint32_t m_SpecDefine1:2;
}PeripheralMapTypeDef;


typedef struct
{
	uint8_t FamilyType;   
	uint8_t ResetState;   
	uint8_t SiliconRev;    
	uint16_t PinCnt;       
	uint32_t PFlashSize;  
	uint32_t FlexNVMSize; 
	uint32_t RAMSize;     
	uint32_t CoreClock;   
	uint32_t BusClock;    
	uint32_t FlexBusClock;
	uint32_t FlashClock;   
} CPUInfoType_t;

extern CPUInfoType_t CPUInfo;


#define ClockSource_IRC     ((uint8_t)0x0)
#define ClockSource_EX8M  	((uint8_t)0x1)
#define ClockSource_EX50M   ((uint8_t)0x2)


#define CoreClock_48M		((uint8_t)0x4)
#define CoreClock_24M		((uint8_t)0x3)
#define CoreClock_96M       ((uint8_t)0x7)
#define CoreClock_80M       ((uint8_t)0x6)
#define CoreClock_64M       ((uint8_t)0x5)
#define CoreClock_100M      ((uint8_t)0x8)
#define CoreClock_110M      ((uint8_t)0x9)
#define CoreClock_120M      ((uint8_t)0x10)

#define NVIC_PriorityGroup_0         ((uint32_t)0x7)                                               
#define NVIC_PriorityGroup_1         ((uint32_t)0x6)                                                 
#define NVIC_PriorityGroup_2         ((uint32_t)0x5)                                                   
#define NVIC_PriorityGroup_3         ((uint32_t)0x4)                                                 
#define NVIC_PriorityGroup_4         ((uint32_t)0x3)


void SystemClockSetup(uint8_t ClockOption,uint16_t CoreClock);  
void SystemSoftReset(void);                                     
void GetCPUInfo(void);                                          
void EnableInterrupts(void);                                    
void DisableInterrupts(void);                                   
void SetVectorTable(uint32_t offset);                           
void NVIC_EnableIRQ(IRQn_Type IRQn);                                          
void NVIC_DisableIRQ(IRQn_Type IRQn);                           
void NVIC_Init(IRQn_Type IRQn,uint32_t PriorityGroup,uint32_t PreemptPriority,uint32_t SubPriority); //设置中断优先级
void PinMuxConfig(uint8_t GPIOIndex, uint8_t PinIndex, uint8_t MuxIndex);
uint16_t GetFWVersion(void);

#endif

