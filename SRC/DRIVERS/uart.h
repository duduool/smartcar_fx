
#ifndef __UART_H__
#define __UART_H__
#include "sys.h"

//串口初始化
typedef struct
{
  uint32_t UART_BaudRate;      //波特率
	uint32_t UARTxMAP;           //初始化端口
} UART_InitTypeDef;

//初始化端口，名字中包含的引脚名字即是其所需要的引脚号
#define UART0_RX_PE21_TX_PE20   (0xA920U)
#define UART0_RX_PA01_TX_PA02   (0x8280U)
#define UART0_RX_PA15_TX_PA14   (0x9CC0U)
#define UART0_RX_PB16_TX_PB17   (0xA0C8U)
#define UART0_RX_PD06_TX_PD07   (0x8CD8U)
#define UART1_RX_PE01_TX_PE00   (0x80E1U)
#define UART1_RX_PA18_TX_PA19   (0xA4C1U)
#define UART1_RX_PC03_TX_PC04   (0x86D1U)
#define UART2_RX_PE17_TX_PE16   (0xA0E2U)
#define UART2_RX_PE23_TX_PE22   (0xAD22U)
#define UART2_RX_PD02_TX_PD03   (0x84DAU)
#define UART2_RX_PD04_TX_PD05   (0x88DAU)


#define UART_IT_TDRE        (uint16_t)(0)
#define UART_IT_TC          (uint16_t)(1)
#define UART_IT_RDRF        (uint16_t)(2)
#define UART_IT_IDLE        (uint16_t)(3)

												
#define UART_DMAReq_Tx                      ((uint16_t)0)
#define UART_DMAReq_Rx                      ((uint16_t)1)
#define UART_DMAREQ(REQ)  (((REQ) == UART_DMAReq_Tx) || ((REQ) == UART_DMAReq_Rx))


void UART_Init(UART_InitTypeDef* UART_InitStruct);
void UART_printf(char *fmt, ...);
void UART_PortInit(uint32_t UARTxMAP,uint32_t UART_BaudRate);
void DisplayCPUInfo(void);
void UART_SendData(void *UARTx,uint8_t Data);
uint8_t UART_ReceiveData(void *UARTx,uint8_t *ch);
void UART_ITConfig(void* UARTxx, uint16_t UART_IT, FunctionalState NewState);
ITStatus UART_GetITStatus(void* UARTxx, uint16_t UART_IT);
uint8_t Normalized_U8(uint16_t data);

#endif
