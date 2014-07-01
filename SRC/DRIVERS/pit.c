#include "pit.h"

//=================================================================================================
// ��ʼ��PITģ��
//=================================================================================================
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct)
{	 
	uint32_t time = 0;
	//�����Ƶ��
	GetCPUInfo();
	time = (PIT_InitStruct->PIT_Interval)*(CPUInfo.BusClock/1000);
	//ʹ��PITʱ��
	SIM->SCGC6|=SIM_SCGC6_PIT_MASK;
	//ʹ��ģ��
	PIT->MCR&=~PIT_MCR_MDIS_MASK;
	//��ʼ��ʱ
	PIT->MCR|=PIT_MCR_FRZ_MASK;
	
	PIT->CHANNEL[PIT_InitStruct->PITx].LDVAL = (uint32_t)time;//������ʱʱ��
	PIT->CHANNEL[PIT_InitStruct->PITx].TFLG =0;//|= PIT_TFLG_TIF_MASK;//�����־λ
	PIT->CHANNEL[PIT_InitStruct->PITx].TCTRL|= (PIT_TCTRL_TEN_MASK);//������ʱ�����ж�
}


//=================================================================================================
// PIT ���Ԥ�趨ʱ��ֵ��С
//=================================================================================================
uint32_t PIT_GetLoadValue(uint8_t PITx)
{
	return 	PIT->CHANNEL[PITx].LDVAL;
}


//=================================================================================================
// PIT ��õ�ǰ���ж�ʱ��ֵ��С
//=================================================================================================
uint32_t PIT_GetCurrentValue(uint8_t PITx)
{	
	return PIT->CHANNEL[PITx].CVAL;
}


//=================================================================================================
// PIT ����Ԥ��ֵ��С
//=================================================================================================
void PIT_SetLoadValue(uint8_t PITx, uint32_t Value)
{
	PIT->CHANNEL[PITx].LDVAL = Value;
}


//=================================================================================================
// PIT ��ʼ����
//=================================================================================================
void PIT_Start(uint8_t PITx)
{
	PIT->CHANNEL[PITx].TCTRL |= PIT_TCTRL_TEN_MASK;
}


//=================================================================================================
// PIT ��ͣ
//=================================================================================================
void PIT_Stop(uint8_t PITx)
{
	PIT->CHANNEL[PITx].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}


//=================================================================================================
// PIT �ж�����
//=================================================================================================
void PIT_ITConfig(uint8_t PITx, uint16_t PIT_IT, FunctionalState NewState)
{
	if(PIT_IT == PIT_IT_TIF)
	{
		(ENABLE == NewState)?(PIT->CHANNEL[PITx].TCTRL |= PIT_TCTRL_TIE_MASK):(PIT->CHANNEL[PITx].TCTRL &= ~PIT_TCTRL_TIE_MASK);
	}
}


//=================================================================================================
//PIT ����жϱ�־
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
// PIT ����жϱ�־
//=================================================================================================
void PIT_ClearITPendingBit(uint8_t PITx,uint16_t PIT_IT)
{
	PIT->CHANNEL[PITx].TFLG |= PIT_TFLG_TIF_MASK;
}