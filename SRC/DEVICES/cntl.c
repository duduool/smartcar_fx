#include "cntl.h"
#include "led.h"
#include "stdio.h"
#include "uart.h"

/* const value */
const uint8_t CENTER = 57;  // 小车中心
const uint8_t THRESHOLD = 0xF5; 
const uint8_t YUZI = 45;
                            // 阈值
/* global variable */
uint8_t IMAGE[COL];         // 图像的一帧
uint8_t OFFSET = 57;        // 偏移

/* global status */
int m[] = {CENTER, CENTER, CENTER, CENTER}; 
                            // 依次存放当前中值，上次，上上次，上上上次中值
int servo;
int flag;
int MID;
int prevMID;

/* pid variable */
double Kp = 1.42;            // PID p控制
double Ki = 0;               // PID i控制
double Kd = 0.12;            // PID d控制

const uint16_t duty[] = {   // 舵机输出PWM
   1794, 1803, 1812, 1821, 1830, 1839, 1848, 1857, // 0 
   1866, 1875, 1884, 1893, 1902, 1911, 1920, 1929, // 8
   1938, 1947, 1956, 1965, 1974, 1983, 1992, 2001, // 16
   2010, 2019, 2028, 2037, 2046, 2055, 2064, 2073, // 24
   2082, 2091, 2100, 2109, 2118, 2127, 2136, 2145, // 32
   2154, 2163, 2172, 2181, 2190, 2199, 2208, 2217, // 40
   2226, 2235, 2244, 2253, 2262, 2271, 2280, 2289, // 48
   2298, 2307, 2316, 2325, 2344, 2344, 2344, 2344, // 56
   2344, 2344, 2344, 2397, 2406, 2415, 2424, 2433, // 64
   2442, 2451, 2460, 2469, 2478, 2487, 2496, 2505, 
   2514, 2523, 2532, 2541, 2550, 2559, 2568, 2577, 
   2586, 2595, 2604, 2613, 2622, 2631, 2640, 2649, 
   2658, 2667, 2676, 2685, 2694, 2703, 2712, 2721, 
   2730, 2739, 2748, 2757, 2766, 2775, 2784, 2793, 
   2802, 2811, 2820, 2829, 2838, 2847, 2849, 2849, 
   2849, 2849, 2849, 2849, 2849, 2849, 2849, 2849, 
};      

/* 中值滤波 */
void Mid_Filter(void)
{           
  uint8_t a[5];
  uint8_t i, j, k, temp;

  for (k = 3; k < 125; k++) {
    a[0] = IMAGE[k-2];
    a[1] = IMAGE[k-1];
    a[2] = IMAGE[k];
    a[3] = IMAGE[k+1];
    a[4] = IMAGE[k+2];
       
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4-i; j++) {
            if (a[j] > a[j+1]) {
                temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
            }
        }
    }
    IMAGE[k] = a[2]; 
  }  
}

/* 寻找左边线 */
static uint8_t get_leftboder(int start)
{
    uint8_t i = 0, left = 0;
    for (i = start; i > 7; i--) {
        if (IMAGE[i] > (IMAGE[i-6]+YUZI) && IMAGE[i-1] > (IMAGE[i-7]+YUZI)) {
               left = i - 5;
               break;
        }
    }
    
    return left;
} 

/* 寻找右边线 */
static uint8_t get_rightboder(int start) 
{
    uint8_t i = 0, right = 127;
    for (i = start; i < 120; i++) {
          if (IMAGE[i] > (IMAGE[i+6]+YUZI) && IMAGE[i+1] > (IMAGE[i+7]+YUZI)) {
               right = i + 5;
               break;
          }
    }
    
    return right;
}

/* 提取赛道中线 */
void Get_Mid(void)
{
    uint8_t left = get_leftboder(MID);
    uint8_t right= get_rightboder(MID);
    
    if (right < 49) {                           // U型
        left = right;
        right = 127;
    }
    
    if (left != 0 && right != 127) {            // 直道
        flag = 0;
        MID = (left + right) >> 1;
        Kp = Kd = 0;
    } else if (left == 0 && right != 127) {     // 左拐
        if (right >= 50 && right < 70) {        // 大弯
            OFFSET = 45;
        } else if (right >= 70 && right < 95) { // 急弯
            OFFSET = 50;
        }
        MID = right - OFFSET;
        flag = -1;
        Kp = 1.43;
        Kd = 0.12;
    } else if (left != 0 && right == 127) {     // 右拐
        if (left < 13) {                        // 大弯
            OFFSET = 30;
        } else if (left >= 13 && left < 60) {   // 急弯
            OFFSET = 40;
        } else {
            OFFSET = 42;
        }
        MID = left + OFFSET;
        flag = 1;
        Kp = 1.35;
        Kd = 0.11;
    } else {                                     // 十字
        if (flag == -1) {                        // 入十字前是左弯
             Steer_Out(duty[64 + servo]);
        } else if (flag == 0) {                  // 入十字前是直道
             Steer_Out(duty[64]);
        } else if (flag == 1){                   // 入十字前是右弯
             Steer_Out(duty[64 + servo]);
        }
    }
    prevMID = MID;
}

/* 控制舵机 */
void Steer_PIDx(void) 
{
    int e[4];
    int ed;
    
    m[3] = MID;  // 存放当前中值
	
    e[3] = m[3] - CENTER;
    e[2] = m[2] - CENTER;
    e[1] = m[1] - CENTER;
    e[0] = m[0] - CENTER;   
    ed = e[3] - e[0];
    
    servo = (e[3])*Kp + (e[3]+e[2]+e[1]+e[0])*Ki + (ed)*Kd;
      
    if (servo > 63) {               
        servo = 63;
    } else if (servo < -64){
        servo = -64;
    }
    
    Steer_Out(duty[64 + servo]);
   
    m[0] = m[1];
    m[1] = m[2];
    m[2] = m[3]; 
}
