#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "sys.h"
#include "delay.h"
#include "pwm.h"
#include "ccd.h"

#define ABS(x)      ((x)>0?(x):-(x)) 
#define MAX(x, y)   (((x) > (y)) ? (x) : (y))
#define MIN(x, y)   (((x) > (y)) ? (y) : (x))

/* APIs */
void Steer_PIDx(uint8_t mid);
void Mid_Filter(void);
void Binarization(void);
uint8_t Average(void);
uint8_t Get_MidLine(void);

#endif /*_CONTROL_H_*/
