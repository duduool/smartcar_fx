#include "cntl.h"

/* const value */
const uint8_t CENTER =  64; // 小车中心
const uint8_t OFFSET =  59; // 丢线后中线的偏移

/* global variable */
extern 
uint8_t IMAGE[COL];         // 二值化后的像素点
uint8_t Mid[6];             // 最近六次中点

/* global status */
uint8_t crosscnt;           // 双线丢失行数判定十字路    
uint8_t findcross;          // 发现十字路

/* pid variable */
double Kp = 2.1;            // PID p控制
double Ki = 0;              // PID i控制
double Kd = 0.5;            // PID d控制
int m[] = {64, 64, 64, 64}; // 依次存放当前中值，上次，上上次，上上上次中值
uint16_t duty[] = {         // 128点每个点对应的舵机值
   1794, 1803, 1812, 1821, 1830, 1839, 1848, 1857, // 0   ~ 7
   1866, 1875, 1884, 1893, 1902, 1911, 1920, 1929, // 8   ~ 15
   1938, 1947, 1956, 1965, 1974, 1983, 1992, 2001, // 16  ~ 23
   2010, 2019, 2028, 2037, 2046, 2055, 2064, 2073, // 24  ~ 31
   2082, 2091, 2100, 2109, 2118, 2127, 2136, 2145, // 32  ~ 39
   2154, 2163, 2172, 2181, 2190, 2199, 2208, 2217, // 40  ~ 47
   2226, 2235, 2244, 2253, 2262, 2271, 2280, 2289, // 48  ~ 55
   2298, 2307, 2316, 2325, 2334, 2343, 2344, 2344, // 56  ~ 63
   2344, 2379, 2388, 2397, 2406, 2415, 2424, 2433, // 64  ~ 71
   2442, 2451, 2460, 2469, 2478, 2487, 2496, 2505, // 72  ~ 79
   2514, 2523, 2532, 2541, 2550, 2559, 2568, 2577, // 80  ~ 87
   2586, 2595, 2604, 2613, 2622, 2631, 2640, 2649, // 88  ~ 95
   2658, 2667, 2676, 2685, 2694, 2703, 2712, 2721, // 96  ~ 103
   2730, 2739, 2748, 2757, 2766, 2775, 2784, 2793, // 104 ~ 111
   2802, 2811, 2820, 2829, 2838, 2847, 2849, 2849, // 112 ~ 119
   2849, 2849, 2849, 2849, 2849, 2849, 2849, 2849, // 120 ~ 127
};

/* 找左黑线 */
static uint8_t leftblackline() {
    uint8_t i;
    uint8_t left;
    for (i = 64; i > 0; i--) {
        if (i <= 1) {
            left = 0;
            break;
        }
        if (IMAGE[i] == 1 && IMAGE[i-1] == 1) {
            left = i;
            break;
        }
    }
    return left;
}

/* 找右黑线 */
static uint8_t rightblackline() {
    uint8_t i;
    uint8_t right;
    for (i = 64; i < 127; i++) {
        if (i >= 126) {
            right = 127;
            break;
        }
        if (IMAGE[i] == 1 && IMAGE[i+1] == 1) {
            right = i;
            break;
        }
    }
    return right;
}


/* 拟合中心引导线 */
uint8_t Get_MidLine(void)
{
    //uint8_t left = leftblackline();
    uint8_t right= rightblackline();
    uint8_t left = right;
    uint8_t mid = CENTER;
    if (left > 2 && right < 125)
        mid = (left + right) >> 2;
    else if (left > 2 && right >= 125)
        mid = left + OFFSET;
    else if (left < 2 && right < 125)
        mid = right - OFFSET;
    else 
        mid = mid;
    
    return mid;
}


/* 在二值化之前滤波 */
void Mid_Filter(void)
{           
    int i, old = 0, average, mid, temp = 0;
    IMAGE[0] = IMAGE[2];             
    IMAGE[1] = IMAGE[2];
    for(i = 1; i < 127; i++){                     
        if(old == IMAGE[i+1]){
             IMAGE[i] = temp; 
        }else{
              if((IMAGE[i-1] > IMAGE[i] && IMAGE[i-1] < IMAGE[i+1]) || (IMAGE[i-1] > IMAGE[i+1] && IMAGE[i-1] < IMAGE[i])) {
                   mid = IMAGE[i-1];
              } else if (  (IMAGE[i] > IMAGE[i-1] && IMAGE[i] < IMAGE[i+1]) || (IMAGE[i]>IMAGE[i+1] && IMAGE[i] < IMAGE[i-1])) {
                   mid = IMAGE[i];
              } else if ((IMAGE[i+1] > IMAGE[i] && IMAGE[i+1] < IMAGE[i-1]) || (IMAGE[i+1]>IMAGE[i-1] && IMAGE[i+1] < IMAGE[i])) {
                   mid = IMAGE[i+1];
              } else {
                   mid = IMAGE[i];
              }
              average = (IMAGE[i-1] + IMAGE[i] + IMAGE[i+1])/3;
              temp = 0.3*mid + 0.7*average;
              IMAGE[i] = temp;
        }
        old = IMAGE[i-1];
    } 
}

/* 简单的固定阈值二值化函 */
void Binarization(void)
{
	unsigned char i;
	/* 固定阈值二值化(最好先滤波) */
    for (i = 0; i < COL; i++) {
        if(IMAGE[i] > 65) 
            IMAGE[i] = 1;    // 白
         else 
            IMAGE[i] = 0;    // 黑
     }
}

/* 求平均值 */
uint8_t Average()
{
    uint8_t i;
    uint8_t sum = 0;
    for (i = 0; i < 6; i++) {
        sum += Mid[i];
    }
    sum /= 6;
    
    return sum;
}

/* PID算法控制舵机打角 */
void Steer_PIDx(uint8_t mid) 
{
    int temp;
    int e[4];
    int ed;
      
    m[3] = mid;
      
	e[3] = m[3] - CENTER;
    e[2] = m[2] - CENTER;
    e[1] = m[1] - CENTER;
    e[0] = m[0] - CENTER;
    ed = e[3]-e[0];
    
    temp = (e[3])*Kp + (e[3]+e[2]+e[1]+e[0])*Ki + (ed)*Kd;
      
    if(temp > 63){               
        temp = 63;
    }else if(temp < -64){
        temp = -64;
    }
      
    Steer_Out(duty[CENTER + temp]);
      
    if(temp < 10 && temp > -10) {
      //  Motor_Forward(DEF_SPEED+15);
    }else if(temp < 25 && temp > -25) {
      //  Motor_Forward(DEF_SPEED);
    }else{
      //  Motor_Forward(DEF_SPEED - 15);
    }
      
    m[0] = m[1];
    m[1] = m[2];
    m[2] = m[3]; 
}
