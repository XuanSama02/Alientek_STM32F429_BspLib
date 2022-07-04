#ifndef _BSP_DELAY_TIM_H_
#define _BSP_DELAY_TIM_H_

#include "main.h"
#include "tim.h"

/*
STM32_HAL RTOS Delay XuanLib
2022/06/17
changelog：
1.0：利用TIM10实现FreeRTOS微秒级延时
*/

void Delay_us(uint16_t num);

#endif
