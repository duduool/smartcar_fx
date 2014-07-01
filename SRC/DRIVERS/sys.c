#include "sys.h"
CPUInfoType_t CPUInfo; // struct to record important CPU information

extern uint32_t SystemCoreClock;

void GetCPUInfo(void)
{
	//CPU 类型
	switch((SIM->SDID & SIM_SDID_FAMID_MASK)>>SIM_SDID_FAMID_SHIFT) 
	{  
    	case 0x0: CPUInfo.FamilyType = 0; break; //KL0x Family(low end)
    	case 0x1: CPUInfo.FamilyType = 1; break; //KL1x Family(basic)
		case 0x2: CPUInfo.FamilyType = 2; break; //KL2x Family(USB)
        case 0x3: CPUInfo.FamilyType = 3; break; //KL3x Family(segment LCD)
		case 0x4: CPUInfo.FamilyType = 4; break; //KL4x Familt(USB and segment LCD)
		default: CPUInfo.FamilyType = 255;break;
	}
	//Pin Package
	switch((SIM->SDID & SIM_SDID_PINID_MASK)>>SIM_SDID_PINID_SHIFT) 
	{  
		
		case 0x00: CPUInfo.PinCnt = 16;  break;
		case 0x01: CPUInfo.PinCnt = 24;  break;
		case 0x02: CPUInfo.PinCnt = 32;  break;
		case 0x03: CPUInfo.PinCnt =  0;  break;	
		case 0x04: CPUInfo.PinCnt = 48;  break;	
		case 0x05: CPUInfo.PinCnt = 64;  break;	
		case 0x06: CPUInfo.PinCnt = 80;  break;	
		case 0x07: CPUInfo.PinCnt =  0;  break;	
		case 0x08: CPUInfo.PinCnt =100;  break;	
		case 0x09: CPUInfo.PinCnt =  0;  break;	
		case 0x0A: CPUInfo.PinCnt =  0;  break;	
		case 0x0B: CPUInfo.PinCnt =  0;  break;	
		case 0x0C: CPUInfo.PinCnt =  0;  break;	
		case 0x0D: CPUInfo.PinCnt =  0;  break;	
		case 0x0E: CPUInfo.PinCnt =  0;  break;	
		case 0x0F: CPUInfo.PinCnt =  0;  break;	
        default:  CPUInfo.PinCnt = 0;    break;	
	}  
   //SiliconRevID
    switch((SIM->SDID & SIM_SDID_REVID_MASK)>>SIM_SDID_REVID_SHIFT) 
    { 
			case 0x0: CPUInfo.SiliconRev = 10;break;
			case 0x1: CPUInfo.SiliconRev = 11;break;
			case 0x2: CPUInfo.SiliconRev = 12;break;
			default:  CPUInfo.SiliconRev = 0; break;
    }	
	//PFlash 大小
	switch((SIM->FCFG1 & SIM_FCFG1_PFSIZE_MASK)>>SIM_FCFG1_PFSIZE_SHIFT)
	{
		case 0x01: CPUInfo.PFlashSize = 8*1024;  break;
		case 0x02: CPUInfo.PFlashSize = 16*1024; break;
		case 0x03: CPUInfo.PFlashSize = 32*1024; break;
		case 0x04: CPUInfo.PFlashSize = 0*1024;  break;
		case 0x05: CPUInfo.PFlashSize = 64*1024; break;
		case 0x06: CPUInfo.PFlashSize = 0*1024;  break;
		case 0x07: CPUInfo.PFlashSize = 128*1024;break;
		case 0x08: CPUInfo.PFlashSize = 0*1024;  break;
		case 0x09: CPUInfo.PFlashSize = 256*1024;break;
		case 0x0A: CPUInfo.PFlashSize = 0*1024;  break;
		case 0x0B: CPUInfo.PFlashSize = 0*1024;  break;
		case 0x0C: CPUInfo.PFlashSize = 0*1024;  break;
		case 0x0D: CPUInfo.PFlashSize = 0*1024;  break;	
		case 0x0E: CPUInfo.PFlashSize = 0*1024;  break;	
		case 0x0F: CPUInfo.PFlashSize = 256*1024;break;	
	 	default:  CPUInfo.PFlashSize = 0*1024;   break; 		
	}
	//SRAM大小
	switch((SIM->SDID & SIM_SDID_SRAMSIZE_MASK)>>SIM_SDID_SRAMSIZE_SHIFT)
	{
		case 0x00: CPUInfo.RAMSize = 512;     break;
		case 0x01: CPUInfo.RAMSize = 1*1024;  break;
		case 0x02: CPUInfo.RAMSize = 2*1024;  break;
		case 0x03: CPUInfo.RAMSize = 4*1024;  break;
		case 0x04: CPUInfo.RAMSize = 8*1024;  break;
		case 0x05: CPUInfo.RAMSize = 16*1024; break;
		case 0x06: CPUInfo.RAMSize = 32*1024; break;
		case 0x07: CPUInfo.RAMSize = 64*1024; break;
		default:break;
	}
	//时钟配置
	CPUInfo.CoreClock = SystemCoreClock;
	CPUInfo.BusClock=(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT)+1));     
	CPUInfo.FlashClock = (SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT)+1));   
	CPUInfo.FlexBusClock = 0;
	CPUInfo.BusClock = (SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT)+1));  
	
}


void PinMuxConfig(uint8_t GPIOIndex, uint8_t PinIndex, uint8_t MuxIndex)
{
	switch(GPIOIndex)
	{
		case 0:
			SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
			PORTA->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
			PORTA->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
			break;
		case 1:
			SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
			PORTB->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
			PORTB->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
			break;
		case 2:
			SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;
			PORTC->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
			PORTC->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
			break;
		case 3:
			SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
			PORTD->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
			PORTD->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
			break;
		case 4:
			SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
			PORTE->PCR[PinIndex]&= ~(PORT_PCR_MUX_MASK);    
			PORTE->PCR[PinIndex]|= PORT_PCR_MUX(MuxIndex); 
			break;
		default:
			break;
	}
}


//=================================================================================================
// 设置 更改系统主频
// 入口： 时钟源选择
//			  ClockSource_IRC    : 内部时钟源
//			  ClockSource_EX50M  : 外部50M晶振
//	  	 CoreClock内核频率
//		      CoreClock_48M  : CoreClock = 48M
//		      CoreClock_64M  : CoreClock = 64M
//			  CoreClock_72M  : CoreClock = 72M
//			  CoreClock_96M  : CoreClock = 96M
//			  CoreClock_100M : CoreClock = 100M
//            CoreClock_11M  : CoreClock = 110M
//            CoreClock_120M : CoreClock = 120M
//可自行配置，但经测试，总线时钟不可超过40M，40M下8秒内必挂无疑
//=================================================================================================
void SystemClockSetup(uint8_t ClockOption,uint16_t CoreClock)
{
	//====使用内部时钟源====//
	if(ClockOption == ClockSource_IRC)
	{
		SIM->CLKDIV1 = (uint32_t)0x00020000UL; 
		MCG->C1 |= (MCG_C1_IREFS_MASK | MCG_C1_IRCLKEN_MASK);
		MCG->C2 = 0;
		MCG->C4 &= ~MCG_C4_DRST_DRS_MASK;
		switch(CoreClock)
		{
			case CoreClock_48M:
				MCG->C4 |= (MCG_C4_DMX32_MASK|MCG_C4_DRST_DRS(1));
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(1));
				SystemCoreClock = 48000000;
				break;				
			case CoreClock_24M:	
				MCG->C4 |= (MCG_C4_DMX32_MASK|MCG_C4_DRST_DRS(0));
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(0));
				SystemCoreClock = 24000000;
				break;		
		}
		OSC0->CR = (uint8_t)0x80U;
		MCG->C5 = (uint8_t)0x00U;
		MCG->C6 = (uint8_t)0x00U;
		while((MCG->S & MCG_S_IREFST_MASK) == 0x00U) {};/* 确认FLL参考时钟是内部时钟. */
		while((MCG->S & 0x0CU) != 0x00U) {};    /* 等待FLL被选择 */
	}
	
	//====使用外部50M时钟源====//
	if(ClockOption == ClockSource_EX50M)
	{
		SIM->SCGC5 |= (uint32_t)0x0200UL; /*  开启 PORTA 时钟门 */   
 
		SIM->CLKDIV1 = (uint32_t)0x10010000UL; /*  内核/系统2分频；Flash和总线2分频 */
		
		/* PORTA->PCR18: ISF=0,MUX=0 */
		PORTA->PCR[18] &= (uint32_t)~0x01000700UL;
		
		/* PORTA->PCR19: ISF=0,MUX=0 */
		PORTA->PCR[19] &= (uint32_t)~0x01000700UL;
		
		/* Switch to FBE Mode */
		/* OSC0->CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=1,SC4P=0,SC8P=0,SC16P=1 */	
		OSC0->CR = (uint8_t)0x89U; //外部参考使能；
		
		/* MCG->C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
		MCG->C2 = (uint8_t)0x34U; //选择使用外部参考源；使用8-32Mhz晶振
		
		/* MCG->C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
		MCG->C1 = (uint8_t)0xBAU; //FLL外部参考,1536分频
		
		/* MCG->C4: DMX32=0,DRST_DRS=0 */
		MCG->C4 &= (uint8_t)~(uint8_t)0xE0U;
		
		/* MCG->C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
		MCG->C5 = MCG_C5_PRDIV0(24);// PLL外部参考,25分频	（寄存器自加1，设置0为1分频，1为2分频，2为3分频，n为n+1分频）
		
        switch(CoreClock)
		{
			case CoreClock_48M:
				MCG->C6 = MCG_C6_VDIV0(0); //2M*24 = 48M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(1)); //Core=48M/1  Bus=48M/1/2   n+1倍分频
				SystemCoreClock = 48000000;
				break;				
			case CoreClock_24M:	
				MCG->C6 = MCG_C6_VDIV0(0); //2M*24 = 48M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(1) | SIM_CLKDIV1_OUTDIV4(0)); //Core=48M/2  Bus=48M/2/1    n+1倍分频
				SystemCoreClock = 24000000;
				break;		
			case CoreClock_96M:	
				MCG->C6 = MCG_C6_VDIV0(24); //2M*48 = 96M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(2)); //Core=96M/1  Bus=96M/1/3=32M    n+1倍分频
				SystemCoreClock = 96000000;
				break;	
			case CoreClock_80M:	
				MCG->C6 = MCG_C6_VDIV0(16); //2M*40 = 80M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(2)); //Core=80M/1  Bus=80M/1/3=26.667m    n+1倍分频
				SystemCoreClock = 80000000;
				break;	
			case CoreClock_64M:	
				MCG->C6 = MCG_C6_VDIV0(8); //2M*32 = 64M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(1)); //Core=64M/1  Bus=64M/1/2=32M    n+1倍分频
				SystemCoreClock = 64000000;
				break;
			case CoreClock_100M:	
				MCG->C6 = MCG_C6_VDIV0(26); //2M*50 = 100M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(2)); //Core=100M/1  Bus=100M/1/3=33.33M    n+1倍分频
				SystemCoreClock = 100000000;
				break;
			case CoreClock_110M:	
				MCG->C6 = MCG_C6_VDIV0(31); //2M*55 = 110M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(2)); //Core=110M/1  Bus=110M/1/3=36.67M    n+1倍分频
				SystemCoreClock = 110000000;
				break;
			case CoreClock_120M:	
				MCG->C5 = MCG_C5_PRDIV0(19); //20分频2.5M
				MCG->C6 = MCG_C6_VDIV0(24); //2.5M*48 = 120M   此寄存器自加24，设置0则为24倍频
				SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(3)); //Core=120M/1  Bus=120M/1/4=30M    n+1倍分频
				SystemCoreClock = 120000000;
				break;
		}

				
		while((MCG->S & MCG_S_IREFST_MASK) != 0x00U) {}; /* Check that the source of the FLL reference clock is the external reference clock. */
		while((MCG->S & 0x0CU) != 0x08U) {};    /* Wait until external reference clock is selected as MCG output */
		/* Switch to PBE Mode */
		/* MCG->C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=0 */
		MCG->C6 |= (uint8_t)0x40U;
		while((MCG->S & 0x0CU) != 0x08U) {};    /* Wait until external reference clock is selected as MCG output */
		while((MCG->S & MCG_S_LOCK0_MASK) == 0x00U) {}; /* Wait until locked */
		/* Switch to PEE Mode */
		/* MCG->C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
		MCG->C1 = (uint8_t)0x1AU;
		while((MCG->S & 0x0CU) != 0x0CU) {};    /* Wait until output of the PLL is selected */				
	}

	GetCPUInfo();//获取CPU信息 重新计算各个总线频率
}


////=================================================================================================
// 设置一个中断的优先级
// 入口：IRQn 中断号
//			 PriorityGroup 优先级分组 
//		     NVIC_PriorityGroup_0
//			 NVIC_PriorityGroup_1
//		     NVIC_PriorityGroup_2
//			 NVIC_PriorityGroup_3
//		     NVIC_PriorityGroup_4
//		 	 PreemptPriority  抢占优先级
//			 SubPriority      子优先级(响应优先级)
// 如果两个中断的抢占优先级和响应优先级都一样的话 则看哪种中断先发生就先执行
//高优先级的抢占优先级是可以打断低优先级的抢占优先级中断的
//而抢占优先级相同的中断 高优先级的响应优先级不可以打断低优先级的响应优先级 
////=================================================================================================
void NVIC_Init(IRQn_Type IRQn,uint32_t PriorityGroup,uint32_t PreemptPriority,uint32_t SubPriority)
{
    //NVIC_SetPriorityGrouping(PriorityGroup);
	//NVIC_SetPriority(IRQn,NVIC_EncodePriority(PriorityGroup,PreemptPriority,SubPriority));
}


//=================================================================================================
//软复位
//=================================================================================================
void SystemSoftReset(void)
{   
	//SCB->AIRCR =0X05FA0000|(uint32_t)0x04;	 
	NVIC_SystemReset();
} 


//=================================================================================================
// 使能总中断
// 调用  CMSIS Cortex-M Core Function Access 层 函数 在.core_cmFunc.h
//=================================================================================================
void EnableInterrupts(void)
{
	__enable_irq();
}


//=================================================================================================
// 关闭总中断
// 调用  CMSIS Cortex-M Core Function Access 层 函数 在.core_cmFunc.h
//=================================================================================================
void DisableInterrupts(void)
{
	__disable_irq();
}


//=================================================================================================
// 设置中断向量表起始位置
//=================================================================================================
void SetVectorTable(uint32_t VectorOffset)
{
	SCB->VTOR = VectorOffset;  //更改中断向量表偏移地址
}

//=================================================================================================
//获得固件库版本
//返回：固件库版本
//=================================================================================================
uint16_t GetFWVersion(void)
{
	return(LZVERSION);
}
