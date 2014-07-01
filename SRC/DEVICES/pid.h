#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>

#define DEFAULT_LIMIT 20.0
#define PID_STEER_KP  2.0
#define PID_STEER_KI  0.0
#define PID_STEER_KD  300.0

typedef struct {
  float desired;      // 设置要达到的值
  float error;        // 误差
  float prevError;    // 上一次的误差
  float integ;        // 积分和
  float deriv;        // 微分
  float kp;           // 比例系数
  float ki;           // 积分系数
  float kd;           // 微分系数
  float iLimit;       // 积分限
} PID;

typedef struct {
    float Kp;         // 比例常数
    float Ki;         // 积分常数
    float Kd;         // 微分常数
    float Un_1;       // 目标值
    float En_1;       // 上一次的误差
    float En_2;       // 上上此的误差
} PIDx;

/* APIs */
int32_t Steer_Update(int8_t error);
float   Update_PID(PID *pid, const float measured);
void    PID_Init(PID *pid, const float desired, const float kp, const float ki, const float kd);
void    PIDx_Init(void);
float   PIDx_Value(PIDx *pp, int setpoint, int detectpoint);

#endif /*_PID_H_*/
