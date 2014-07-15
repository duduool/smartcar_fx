//======================================================================
//蓝宙电子-MKL25Z128-例程
//版本：V1.0
//实验平台：MKL25Z128系统板
//淘宝店：http://landzo.taobao.com/
//修改：2013.12.15
//使用说明：本文件所描述的是飞思卡尔智能车光电组所使用的线性CCD采集，包括
//时序信号参数的端口初始化，ADC端口初始化。使用前请在ccd.h文件修改你想
//要的端口参数,调用的时候只要把CCDPar地址给CCDInit即可，然后使用CCD_GetImage()
//进行驱动采样
//======================================================================
#include "ccd.h"
#include "delay.h"
#include "adc.h"
#include "uart.h"
#include "stdio.h"

extern uint8_t THRESHOLD;
extern uint8_t IMAGE[COL];       //储存线性CCD的AD采样值

//在此修改端口号           
CCD_ConfigType  CCDParSet={ADC0,ADC_PRECISION_8BIT,SI_GPIO,SI_Pin,CK_GPIO,CK_Pin}; 
//采集端口,采集精度，SI信号端口，SI端口引脚号，CLK信号端口，CLK端口引脚号

//======================================================================
//线性CCD采集初始化
//入口参数：AD采集端口：ADCx
//          精度：Pre
//返回值  ：无
//函数功能：时序信号参数的端口初始化，ADC端口初始化
//======================================================================
void CCDInit(CCD_ConfigType *CCDPar)
{
	GPIO_InitTypeDef GPIO_InitStruct_SI;
	GPIO_InitTypeDef GPIO_InitStruct_CK;
	ADC_InitTypeDef CCDInitStruct;         //定义CCD采集初始化结构体
	
	//ADC端口初始化	
	CCDInitStruct.ADCxMap = (uint32_t)CCDPar->ADCx;          //端口
	CCDInitStruct.ADC_Precision = CCDPar->Pre;               //精度
	
	//时序参数初始化
	GPIO_InitStruct_SI.GPIO_Pin = SI_Pin;
	GPIO_InitStruct_SI.GPIO_InitState = Bit_RESET;
	GPIO_InitStruct_SI.GPIO_IRQMode = GPIO_IT_DISABLE;
	GPIO_InitStruct_SI.GPIO_Mode = GPIO_Mode_OPP;
	GPIO_InitStruct_SI.GPIOx = CCDPar->SIGPIO;

	GPIO_InitStruct_CK.GPIO_Pin = CCDPar->CKPin;
	GPIO_InitStruct_CK.GPIO_InitState = Bit_RESET;
	GPIO_InitStruct_CK.GPIO_IRQMode = GPIO_IT_DISABLE;
	GPIO_InitStruct_CK.GPIO_Mode = GPIO_Mode_OPP;
	GPIO_InitStruct_CK.GPIOx = CCDPar->CKGPIO;
	
	ADC_Init(&CCDInitStruct);              //地址映射
	GPIO_Init(&GPIO_InitStruct_SI);
	GPIO_Init(&GPIO_InitStruct_CK);
}

//======================================================================
//函 数 名：     CCD_GetImage()                      
//入口参数：     无             
//返回值  ：     无
//函数功能：     CCD驱动以及采样                                                  
//======================================================================
void CCD_GetImage(uint32_t ADCx) 
{
	unsigned char i;
    
    SI_1;  //SI端口置1
    CK_1;  //CK端口置1
    SI_0;  //SI端口置0
	
		
    for (i = 0; i < COL; i++) {
    //延时决定了CCD中的电容器积累的电荷量，同时决定了其是否达到饱和状态或足以采集黑线的状态
    //但是时间过长采集周期会变长，所以请使用蓝宙线性CCD上位机，根据实际图像效果调整延时. 
       CK_0;
                                    
       DelayUs(450);                 
                     
       IMAGE[i] =ADC_GetConversionValue(ADCx); 
        
       CK_1;
       DelayUs(450); 
    }
   
    CK_0;
	DelayUs(20); 
}

//串口通信
void SendImage(void)
{
	unsigned char i;
	char buf[5];
	    for (i = 0; i < COL; i++) {
//           if(IMAGE[i] == 1) 
//              UART_SendData((UART0_Type *)UART0,' ');
//           else
//              UART_SendData((UART0_Type *)UART0,'*');

					if(IMAGE[i] > THRESHOLD)
					sprintf(buf, " ");
				else
					sprintf(buf, "%02X ", IMAGE[i]);
				//sprintf(buf, "%02X ", IMAGE[i]);
				UART_printf(buf);
			//UART_SendData((UART0_Type *)UART0,'a' + (IMAGE[i] >> 4));
       }
      UART_printf("\n");
}

uint8_t getThreshold(void)
{
	unsigned char i;
	uint8_t min = IMAGE[0];
	uint8_t max = IMAGE[0];
	for (i = 1; i < COL; i++) {
		if(IMAGE[i] > max)
			max = IMAGE[i];
		else
		{
			if(IMAGE[i] < min)
				min = IMAGE[i];
		}
	}
	UART_printf("\n max=%d  min=%d\n",max,min);
	return (max + min)/2;
}
/*************************************************************************
*  函数名称：SendHex
*  功能说明：采集发数程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendHex(unsigned char hex) 
{
  unsigned char temp;
  temp = hex >> 4;
  if(temp < 10) {
    UART_SendData(UART0, temp + '0');
  } else {
    UART_SendData(UART0, temp - 10 + 'A');
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    UART_SendData(UART0, temp + '0');
  } else {
   UART_SendData(UART0, temp - 10 + 'A');
  }
}

/*************************************************************************
*  函数名称：SendImageData
*  功能说明：
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendImageData(unsigned char * ImageData) 
{
    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
    UART_SendData(UART0, '*');
    UART_SendData(UART0, 'L');
    UART_SendData(UART0, 'D');

    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);

    for(i=0; i<128; i++) {
      SendHex(*ImageData++);
    }

    SendHex(crc);
    UART_SendData(UART0,'#');
}
