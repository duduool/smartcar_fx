#ifndef __ADC_H__
#define __ADC_H__

#include "sys.h"

//声明ADC端口地址
#define ADC0_SE8A_PB0					(0x104048U)
#define ADC0_SE9A_PB1					(0x124248U)

typedef struct
{
    uint32_t ADCxMap;              
	uint32_t ADC_Precision;     //ADC精度
	uint16_t ADC_TriggerSelect; //触发源
}ADC_InitTypeDef;



// 精度
#define ADC_PRECISION_8BIT    (0x00U)
#define ADC_PRECISION_10BIT   (0x02U)
#define ADC_PRECISION_12BIT   (0x01U)
#define ADC_PRECISION_16BIT   (0x03U)


#define ADC_TRIGGER_HW     (uint16_t)(0)
#define ADC_TRIGGER_SW     (uint16_t)(1)


#define ADC_IT_AI          (uint16_t)(0)


#define ADC_DMAReq_COCO    ((uint16_t)0)



#define A                 0x0
#define B                 0x1


void ADC_Init(ADC_InitTypeDef* ADC_InitStruct);
void ADC_DMACmd(ADC_Type* ADCx, uint16_t ADC_DMAReq, FunctionalState NewState);
uint32_t ADC_GetConversionValue(uint32_t ADCxMap);
void ADC_ITConfig(ADC_Type* ADCx,uint8_t ADC_Mux, uint16_t ADC_IT, FunctionalState NewState);
void ADC_ClearITPendingBit(ADC_Type* ADCx, uint8_t ADC_Mux, uint16_t ADC_IT);

#endif

