#ifndef _TPM_H_
#define _TPM_H_

#include "sys.h"
#include "gpio.h"
#include "pid.h"

#define TPM_0               0
#define TPM_1               1
#define TPM_2               2

#define CH0                 0
#define CH1                 1
#define CH2                 2
#define CH3                 3
#define CH4                 4
#define CH5                 5
#define CH6                 6
#define CH7                 7


#define PTE21               22     // 舵机
#define PTB18               23     // FTM2_CH0 电机
#define PTB19               24     // FTM2_CH1

// 分频
#define DIV1                0
#define DIV2                1
#define DIV4                2
#define DIV8                3
#define DIV16               4
#define DIV32               5
#define DIV64               6
#define DIV128              7

#define MAX_SPEED           20
#define MID_SPEED           17
#define MIN_SPEED           15
#define DEF_SPEED           13
#define DED_SPEED           10

/* 舵机转角范围(-32, 32)度 */
#define DIR_MID		        2344            // 0
#define DIR_LEFT	        (DIR_MID - 550) // -32 1794
#define DIR_RIGHT	        (DIR_MID + 505) // 32  2849


/* APIs */
void PWMInit(uint8_t Pin, uint8_t Div, uint16_t modValue);
void PWMOutput(uint8_t Pin, uint16_t Duty);
void PWMPortInit(GPIO_Type *GPIO,uint16_t Pin);
void Steer_Init(void);
void Motor_Init(void);
void Steer_Out(uint16_t Duty);
void Motor_Forward(uint16_t Duty);
void Motor_Backward(uint16_t Duty);

#endif
