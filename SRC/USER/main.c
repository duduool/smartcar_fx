#include "adc.h"
#include "led.h"
#include "pit.h"
#include "isr.h"
#include "pwm.h"
#include "ccd.h"
#include "cntl.h"
#include "uart.h"
#include "delay.h"

/* Core Clock = 100MHZ, Bus Clock = 100/3MHZ
   PWM = 100HZ, MOTOR = 20KHZ    
*/
extern uint8_t IMAGE[COL];  
extern uint8_t Mid[6];
extern uint16_t duty[];

int main(void)
{
    uint8_t i = 0, mid;
    i = i;
    DisableInterrupts();
        
    /* 选择外部50M晶振，并且内核可超频到100M */
    SystemClockSetup(ClockSource_EX50M, CoreClock_100M);
    /* 初始化调试串口 */
    UART_PortInit(UART0_RX_PD06_TX_PD07, 9600);
    /* 延时函数初始化 */
    DelayInit();
    /* 舵机初始化 */
    Steer_Init();
    /* 电机初始化 */
    Motor_Init();
    /* LED PTA14初始化 */
    LEDInit(PTA, 14);
    /* 初始化CCD */
    CCDInit(&CCDParSet);
    /* 打印CPU时钟信息 */
    //DisplayCPUInfo();
    /* 静止2S后启动 */
    //DelayMs(2000);
    
    EnableInterrupts();
    
    /* 设置周期定时中断，每隔1ms产生一次中断 */
    //Config_PIT(PIT0,80);
        
    while(1) {
        #if 1
        /* Sampling CCD data */
        //for (i = 0; i < 6; i++) {
            CCD_GetImage((uint32_t)ADC0_SE8A_PB0);
            Mid_Filter();
         Binarization();
            SendImage();
            mid = Get_MidLine();
        //    Mid[i] = Get_MidLine();
        //}
			  
        /* 方向控制 */
        //Steer_PIDx(Average());
        Steer_Out(duty[ABS(mid - 64)]);
        #else
        /* Tesing steer */
        for (i = 0; i < 128; i++) {
            Steer_Out(duty[i]);
            DelayMs(500);
        }
        #endif
    }
}
