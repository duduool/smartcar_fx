#include "pwm.h"

//======================================================================
// 入口参数：GPIO:PTA:A端口 PTB:B端口 PTC:C端口 PTD:D端口 PTE:E端口
//			 Pin：端口第N引脚	 	
// 返回参数：无
// 实现PWM输出前的IO初始化
//======================================================================
void PWMPortInit(GPIO_Type *GPIO, uint16_t Pin){
	GPIO_InitTypeDef GPIO_InitStruct1;
	GPIO_InitStruct1.GPIO_Pin = Pin;
	GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
	GPIO_InitStruct1.GPIOx = GPIO;
	GPIO_Init(&GPIO_InitStruct1);
}

//======================================================================
// 入口参数：Pin ：PWM输出引脚 
//           Div ：时钟源分频数
//           modValue：模值，与初始化的modValue相关，Duty/modValue=实际占空比
//          且其大小决定了PWM输出的频率f=f(bus)/Div/modValue，最大为65535
// 返回参数：无
// 实现初始化PWM
//========================================================================
void PWMInit(uint8_t Pin, uint8_t Div, uint16_t modValue)
{    
	TPM_Type *pstTPMModule;  
	uint8_t  TPMCh;
	//选择TPM时钟源
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);//MCGFLL 或 MCGPLL/2
    // 配置PWM端口   
    switch (Pin)
    {
        case PTB18:
            //PWMPortInit(PTB,18);
            SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
            PORTB->PCR[18] = PORT_PCR_MUX(0x3) | PORT_PCR_DSE_MASK;
            pstTPMModule = TPM2; 
            TPMCh = CH0;
            break;
            
        case PTB19:
            //PWMPortInit(PTB,19);
            SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
            PORTB->PCR[19] = PORT_PCR_MUX(0x3) | PORT_PCR_DSE_MASK; 
            pstTPMModule = TPM2;
            TPMCh = CH1;
            break;
        
        case PTE21:
            //PWMPortInit(PTE,21);
            SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
            PORTE->PCR[21] = PORT_PCR_MUX(0x3) | PORT_PCR_DSE_MASK; 
            pstTPMModule = TPM1;
            TPMCh = CH1;
            break;
     
        default:;
    }
    
// 2. 时钟使能:
    
    if (pstTPMModule == TPM0) {
        SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    } else if (pstTPMModule == TPM1) {
        SIM->SCGC6 |=SIM_SCGC6_TPM1_MASK;
    } else if (pstTPMModule == TPM2) {
        SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
    }
    
// 3. 配置 PWM:
    
    Div &= 0x07; //取低三位，防止误输入大于7的分频数
	pstTPMModule->SC |= //TPM_SC_CPWMS_MASK |   // CPWMS = 1 中心对齐PWM模式    
                        (TPM_SC_CMOD(1) |       // CLKS=1:System clock(it's Bus clock here
                        TPM_SC_PS(Div));        // f[ftm]=f[Bus]/(2^Div)
		
	pstTPMModule->MOD = modValue;               // 这里设置PWM频率，数值越大，占空比可调精度越高，
                                                //但是PWM频率的高低跟电机效率，电机驱动性能，耗电量都相关，请仔细斟酌

	TPM_CnSC_REG(pstTPMModule,TPMCh) = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK; 
                                                //先输出高后输出低      
}

//======================================================================
// 入口参数：Pin ：PWM输出引脚 
//           Duty：占空比，与初始化的modValue相关，Duty/modValue=实际占空比                                   
// 返回参数：无
// 实现更改PWM占空比
//========================================================================
void PWMOutput(uint8_t Pin, uint16_t Duty)
{
    TPM_Type *pstTPMModule;
    uint8_t  TPMCh;
       
    switch (Pin)
    {  
        case PTB18:
            pstTPMModule = TPM2; 
            TPMCh = CH0;
            break;
            
        case PTB19:
            pstTPMModule = TPM2;
            TPMCh = CH1;
            break;
        
        case PTE21:
            pstTPMModule = TPM1;
            TPMCh = CH1;
            break;

        default:break;
    }
     
   TPM_CnV_REG(pstTPMModule,TPMCh) = Duty;
}

/* 初始化起始速度 */
static uint16_t StartSpeed_Init(void)
{
    uint16_t speed = 0;
    
    GPIO_InitTypeDef GPIO_InitStruct1;
    
    GPIO_InitStruct1.GPIO_Pin = 0;
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct1.GPIOx = PTE;
    GPIO_Init(&GPIO_InitStruct1);
    
    GPIO_InitStruct1.GPIO_Pin = 1;
    GPIO_Init(&GPIO_InitStruct1);
    
    GPIO_InitStruct1.GPIO_Pin = 2;
    GPIO_Init(&GPIO_InitStruct1);
    
    GPIO_InitStruct1.GPIO_Pin = 3;
    GPIO_Init(&GPIO_InitStruct1);
    
    if ((uint8_t)1 == GPIO_ReadInputDataBit(PTE, 0)) {
        speed = MAX_SPEED;
    } else if ((uint8_t)1 == GPIO_ReadInputDataBit(PTE, 1)) {
        speed = MID_SPEED;
    } else if ((uint8_t)1 == GPIO_ReadInputDataBit(PTE, 2)) {
        speed = MIN_SPEED;
    } else if ((uint8_t)1 == GPIO_ReadInputDataBit(PTE, 3)) {
        speed = DEF_SPEED;
    } else {
        speed = 0;
    }
    
    return speed;
}


/**********************************************************************/
/* 舵机初始化 */
void Steer_Init(void)
{
    PWMInit(PTE21, 5, 15625); // 舵机频率大概100HZ
    PWMOutput(PTE21, DIR_MID);
}

/* 舵机输出 */
void Steer_Out(uint16_t Duty)
{   
    if (Duty < DIR_LEFT)  Duty = DIR_LEFT;
    if (Duty > DIR_RIGHT) Duty = DIR_RIGHT;
    
    PWMOutput(PTE21, Duty);
}

/* 电机初始化 */
void Motor_Init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[2] &= ~(PORT_PCR_MUX_MASK);
    PORTC->PCR[2] |= PORT_PCR_MUX(1);
    PTC->PDDR |= (1 << 2);
    PTC->PDOR |= (1 << 2);
    
    PWMInit(PTB19, 5, 100); // 电机频率大概20KHZ
    PWMOutput(PTB19, StartSpeed_Init());
}

/* 电机正转，小车前进 */
void Motor_Forward(uint16_t Duty) 
{
    PWMOutput(PTB19, Duty);
}

/* 电机反转，小车后退 */
void Motor_Backward(uint16_t Duty) 
{
    PWMOutput(PTB18, Duty);
}
