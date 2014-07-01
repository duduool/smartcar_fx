#include "uart.h"

void *UART_DebugPort = NULL;


void UART_PortInit(uint32_t UARTxMAP,uint32_t UART_BaudRate)
{
	UART_InitTypeDef UART_DebugInitStruct1;
	PeripheralMapTypeDef *pUART_Map = (PeripheralMapTypeDef*)&(UARTxMAP);
	//配置默认的调试UART串口
	UART_DebugInitStruct1.UART_BaudRate = UART_BaudRate;
	UART_DebugInitStruct1.UARTxMAP = UARTxMAP;
	//找出对应的UART端口
	switch(pUART_Map->m_ModuleIndex)  //uart2 ,portE,23,22
	{
			case 0:
					UART_DebugPort = UART0;
					break;
			case 1:
					UART_DebugPort = UART1;
					break;
			case 2:
					UART_DebugPort = UART2;
					break;
			default:
                    UART_DebugPort = NULL;
					break;
	}
	UART_Init(&UART_DebugInitStruct1);
}


void UART_Init(UART_InitTypeDef* UART_InitStruct)
{
	UART_Type* UARTx = NULL;
	PeripheralMapTypeDef *pUART_Map = NULL;
    uint16_t sbr;
	uint8_t i;
	uint32_t clock;
	uint8_t IsUART0 = FALSE;
	pUART_Map = (PeripheralMapTypeDef*)&(UART_InitStruct->UARTxMAP);
	switch(pUART_Map->m_ModuleIndex)
	{
			case 0:
					SIM->SCGC4|=SIM_SCGC4_UART0_MASK;
					IsUART0 = TRUE;
					break;
			case 1:
					SIM->SCGC4|=SIM_SCGC4_UART1_MASK;
					UARTx = UART1;
					break;
			case 2:
					SIM->SCGC4|=SIM_SCGC4_UART2_MASK;
					UARTx = UART2;
					break;
			default:
					UARTx = NULL;
					break;
	}
	if(IsUART0 == TRUE)
	{
		//uart0是区别于uart1，2的
		SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
		SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);  // PLL/2 或 FLL
		//选择 PLL or FLL 
		if(MCG->C6 & MCG_C6_PLLS_MASK)
		{
			//PLL选择
			SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
			clock = CPUInfo.CoreClock*(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT)+1)>>1;			
		}
		else
		{
			SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
			clock = CPUInfo.CoreClock*(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT)+1);	
		}
		// 禁止TX,RX
		UART0->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
		UART0->C1 = 0;
		
        sbr = (uint16_t)((clock)/((UART_InitStruct->UART_BaudRate)*16));
		UART0->BDH &= ~(UART_BDH_SBR_MASK);
		UART0->BDH |= ((sbr>>8) & UART_BDH_SBR_MASK);
		UART0->BDL = (sbr & UART_BDL_SBR_MASK);
		//使能TX,RX
		UART0->C2 |=(UART_C2_TE_MASK  | UART_C2_RE_MASK );
	}
	else
	{
		// 禁止TX,RX
		UARTx->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
		UARTx->C1 = 0;
		clock = CPUInfo.BusClock;
        sbr = (uint16_t)((clock)/((UART_InitStruct->UART_BaudRate)*16));
		UARTx->BDH &= ~(UART_BDH_SBR_MASK);
		UARTx->BDH |= ((sbr>>8) & UART_BDH_SBR_MASK);
		UARTx->BDL = (sbr & UART_BDL_SBR_MASK);
		//使能TX,RX
		UARTx->C2 |=(UART_C2_TE_MASK  | UART_C2_RE_MASK );
	}
	// 配置复用
	for(i=0;i<pUART_Map->m_PinCntIndex;i++)
	{
		PinMuxConfig(pUART_Map->m_PortIndex,pUART_Map->m_PinBaseIndex+i,pUART_Map->m_MuxIndex);
	}
}


void UART_SendData(void *UARTx, uint8_t Data)
{
	if((uint32_t)UARTx == (uint32_t)UART0) 
	{
		while(!(UART0->S1 & UART_S1_TDRE_MASK));
		UART0->D = (uint8_t)Data;
	}
	else if((uint32_t)UARTx == (uint32_t)UART1)
	{
		while(!(UART1->S1 & UART_S1_TDRE_MASK));
		UART1->D = (uint8_t)Data;
	}
	else if((uint32_t)UARTx == (uint32_t)UART2)
	{
		while(!(UART2->S1 & UART_S1_TDRE_MASK));
		UART2->D = (uint8_t)Data;
	}
}

uint8_t UART_ReceiveData(void *UARTx,uint8_t *ch)
{
	if((uint32_t)UARTx == (uint32_t)UART0) 
	{
		if((UART0->S1 & UART_S1_RDRF_MASK)!= 0)
		{
			*ch = (UART0->D);	
			return TRUE; 		  
		}
	}
	else if((uint32_t)UARTx == (uint32_t)UART1)
	{
		if((UART1->S1 & UART_S1_RDRF_MASK)!= 0)
		{
			*ch = (UART1->D);	
			return TRUE; 		  
		}
	}
	else if((uint32_t)UARTx == (uint32_t)UART2)
	{
		if((UART2->S1 & UART_S1_RDRF_MASK)!= 0)
		{
			*ch = (UART2->D);	
			return TRUE; 		  
		}
	}
	return FALSE;
}

//内部函数为实现UART_printf
static void UART_puts(char *pch)
{
	while(*pch != '\0')
	{
		UART_SendData(UART_DebugPort,*pch);
		pch++;
	}
}
//内部函数为实现UART_printf
static void printn(unsigned int n, unsigned int b)
{
	static char *ntab = "0123456789ABCDEF";
	unsigned int a, m;
	if (n / b)
	{
		a = n / b;
		printn(a, b);  
	}
	m = n % b;
	UART_SendData(UART_DebugPort,ntab[m]);
}


//================================================================================================
// UART 格式化输出
//fmt 输入字符串指针          
//类似于C标准库中的UART_printf 但是只支持 %d %l %o %x %s
//================================================================================================
void UART_printf(char *fmt, ...)
{
    char c;
    unsigned int *adx = (unsigned int*)(void*)&fmt + 1;
_loop:
    while((c = *fmt++) != '%')
		{
        if (c == '\0') return;
        UART_SendData(UART_DebugPort,c);
    }
    c = *fmt++;
    if (c == 'd' || c == 'l')
		{
        printn(*adx, 10);
    }
    if (c == 'o' || c == 'x')
		{
        printn(*adx, c=='o'? 8:16 );
    }
    if (c == 's')
		{
			UART_puts((char*)*adx);
    }
    adx++;
    goto _loop;
}

uint8_t Normalized_U8(uint16_t data)
{
    return (uint8_t)((uint32_t)data*255/4095);
}
//================================================================================================
// 打印处理器信息
//================================================================================================
void DisplayCPUInfo(void)
{
	
	if (RCM->SRS1 & RCM_SRS1_SACKERR_MASK)
		UART_printf("Stop Mode Acknowledge Error Reset\r\n");
	if (RCM->SRS1 & RCM_SRS1_MDM_AP_MASK)
		UART_printf("\n\rMDM-AP Reset\r\n");
	if (RCM->SRS1 & RCM_SRS1_SW_MASK)
		UART_printf("\n\rSoftware Reset\r\n");
	if (RCM->SRS1 & RCM_SRS1_LOCKUP_MASK)
		UART_printf("\n\rCore Lockup Event Reset\r\n");
	
	if (RCM->SRS0 & RCM_SRS0_POR_MASK)
		UART_printf("\n\rPower-on Reset\r\n");
	if (RCM->SRS0 & RCM_SRS0_PIN_MASK)
		UART_printf("\n\rExternal Pin Reset\r\n");
	if (RCM->SRS0 & RCM_SRS0_WDOG_MASK)
		UART_printf("\n\rWatchdog(COP) Reset\r\n");
	if (RCM->SRS0 & RCM_SRS0_LOC_MASK)
		UART_printf("\n\rLoss of External Clock Reset\r\n");
	if (RCM->SRS0 & RCM_SRS0_LOL_MASK)
		UART_printf("\n\rLoss of Lock in PLL Reset\r\n");
	if (RCM->SRS0 & RCM_SRS0_LVD_MASK)
		UART_printf("\n\rLow-voltage Detect Reset\r\n");


	switch(CPUInfo.FamilyType)
	{
		case 0: UART_printf("KL0x Family(low end)\r\n"); break;
		case 1: UART_printf("KL1x Family(basic)\r\n"); break;
		case 2: UART_printf("KL2x Family(USB)\r\n"); break;
		case 3: UART_printf("KL3x Family(Segment LCD)\r\n"); break;
		case 4: UART_printf("KL4x Family(USB and segment LCD)\r\n"); break;
		default: UART_printf("\nUnrecognized Kinetis family device.\n"); break;  
	}
	
	if(CPUInfo.PinCnt == 0)
	{
		UART_printf("Unidentified Pin Cnt\r\n");
	}
	else
	{
		UART_printf("PinCnt:%d\r\n",CPUInfo.PinCnt);
	}
	
	//打印SiliconRevID
	UART_printf("SiliconRevID:%d.%d\r\n",CPUInfo.SiliconRev/10,CPUInfo.SiliconRev%10);
	//打印PFlash大小
	UART_printf("PFlash Size: %dKB\r\n",CPUInfo.PFlashSize/1024);
	//打印FlexNVM大小
	UART_printf("FlexNVM Size: %dKB\r\n",CPUInfo.FlexNVMSize/1024);
	//打印RAM 大小
	UART_printf("RAM Size :%dKB\r\n",CPUInfo.RAMSize/1024);
	//打印CoreClock
	UART_printf("CoreClock: %dHz\r\n",CPUInfo.CoreClock);
	//打印BusClock
	UART_printf("BusClock: %dHz\r\n",CPUInfo.BusClock);
	//打印FlexBusClock
	UART_printf("FlexBusClock: %dHz\r\n",CPUInfo.FlexBusClock);
	//打印FlashClock
	UART_printf("FlashClock: %dHz\r\n",CPUInfo.FlashClock);
}

//================================================================================================
// 配置中断开启
// UART_Type 串口选择
//================================================================================================
void UART_ITConfig(void* UARTxx, uint16_t UART_IT, FunctionalState NewState)
{
	UART_Type* UARTx = NULL;

	if((uint32_t)UARTxx == (uint32_t)UART0) 
	{
		switch(UART_IT)
		{
			case UART_IT_TDRE:
				(ENABLE == NewState)?(UART0->C2 |= UART_C2_TIE_MASK):(UART0->C2 &= ~UART_C2_TIE_MASK);
				break;
			case UART_IT_TC:
				(ENABLE == NewState)?(UART0->C2 |= UART_C2_TCIE_MASK):(UART0->C2 &= ~UART_C2_TCIE_MASK);
				break;
			case UART_IT_RDRF:
				(ENABLE == NewState)?(UART0->C2 |= UART_C2_RIE_MASK):(UART0->C2 &= ~UART_C2_RIE_MASK);
				break;
			case UART_IT_IDLE:
				(ENABLE == NewState)?(UART0->C2 |= UART_C2_ILIE_MASK):(UART0->C2 &= ~UART_C2_ILIE_MASK);
				break;
			default:break;
		}
	}
	else 
	{
		UARTx = (UART_Type*)UARTxx;
		switch(UART_IT)
		{
			case UART_IT_TDRE:
				(ENABLE == NewState)?(UARTx->C2 |= UART_C2_TIE_MASK):(UARTx->C2 &= ~UART_C2_TIE_MASK);
				break;
			case UART_IT_TC:
				(ENABLE == NewState)?(UARTx->C2 |= UART_C2_TCIE_MASK):(UARTx->C2 &= ~UART_C2_TCIE_MASK);
				break;
			case UART_IT_RDRF:
				(ENABLE == NewState)?(UARTx->C2 |= UART_C2_RIE_MASK):(UARTx->C2 &= ~UART_C2_RIE_MASK);
				break;
			case UART_IT_IDLE:
				(ENABLE == NewState)?(UARTx->C2 |= UART_C2_ILIE_MASK):(UARTx->C2 &= ~UART_C2_ILIE_MASK);
				break;
			default:break;
		}
	}
	
}


//================================================================================================
// 获得中断标志
//UART_Type 串口选择
//================================================================================================
ITStatus UART_GetITStatus(void* UARTxx, uint16_t UART_IT)
{
	ITStatus retval;
	UART_Type* UARTx = NULL;

	if((uint32_t)UARTxx == (uint32_t)UART0) 
	{
		switch(UART_IT)
		{
			case UART_IT_TDRE:
				(UART0->S1 & UART_S1_TDRE_MASK)?(retval = SET):(retval = RESET);
				break;
			case UART_IT_TC:
				(UART0->S1 & UART_S1_TC_MASK)?(retval = SET):(retval = RESET);
				break;
			case UART_IT_RDRF:
				(UART0->S1 & UART_S1_RDRF_MASK)?(retval = SET):(retval = RESET);	
				break;
			case UART_IT_IDLE:
				(UART0->S1 & UART_S1_IDLE_MASK)?(retval = SET):(retval = RESET);			
				break;
			default:break;
		}
	}
	else
	{
		UARTx = (UART_Type*)UARTxx;
			switch(UART_IT)
			{
				case UART_IT_TDRE:
					(UARTx->S1 & UART_S1_TDRE_MASK)?(retval = SET):(retval = RESET);
					break;
				case UART_IT_TC:
					(UARTx->S1 & UART_S1_TC_MASK)?(retval = SET):(retval = RESET);
					break;
				case UART_IT_RDRF:
					(UARTx->S1 & UART_S1_RDRF_MASK)?(retval = SET):(retval = RESET);	
					break;
				case UART_IT_IDLE:
					(UARTx->S1 & UART_S1_IDLE_MASK)?(retval = SET):(retval = RESET);			
					break;
				default:break;
			}
	}
	return retval;
}
