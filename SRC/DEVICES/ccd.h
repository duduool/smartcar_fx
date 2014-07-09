#ifndef _CCD_H_
#define _CCD_H_

#include "gpio.h"
#include "sys.h"

//===================================
//CCD时序参数输出端口设定
#include <MKL25Z4.h>
//===================================
#define SI_GPIO PTA
#define CK_GPIO PTA
#define SI_Pin 5
#define CK_Pin 4

#define COL         128

//===================================
//CCD初始化结构体
//===================================
typedef struct{
    ADC_Type *ADCx ;
	uint32_t Pre  ;  
	GPIO_Type *SIGPIO ;
	uint8_t SIPin ;
 	GPIO_Type *CKGPIO ;
	uint8_t CKPin ;
}CCD_ConfigType;

extern CCD_ConfigType  CCDParSet;

//===================================
//CCD时序参数设定
//===================================
#define SI_1  GPIO_SetBits(SI_GPIO, SI_Pin)
#define CK_1  GPIO_SetBits(CK_GPIO, CK_Pin)
#define SI_0  GPIO_ResetBits(SI_GPIO, SI_Pin)
#define CK_0  GPIO_ResetBits(CK_GPIO, CK_Pin)

/* APIs */
void CCDInit(CCD_ConfigType *CCDPar);
void CCD_GetImage(uint32_t ADCx);
uint8_t PixelAverage(uint8_t len, uint8_t *data);
void Binarization(void);
void SendImage(void);

#endif


