#ifndef __LED_H__
#define __LED_H__

#include "gpio.h"
#include "sys.h"

void LEDInit(GPIO_Type *GPIO,uint8_t Pin);
void TwinkleLed(GPIO_Type *GPIO,uint8_t Pin);


#endif



