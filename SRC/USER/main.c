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

int main(void)
{
    uint8_t i = 0;
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
    /* LED PTA17初始化 */
    LEDInit(PTA, 17);
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
        /* Sampling CCD data */
        for (i = 0; i < 1; i++) {
            CCD_GetImage((uint32_t)ADC0_SE8A_PB0);
        }
        //SendImage();
        Mid_Filter();
        Get_Mid();
        Steer_PIDx();
    }
}
