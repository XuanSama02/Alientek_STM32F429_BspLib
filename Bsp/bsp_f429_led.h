#ifndef _BSP_F429_LED_H_
#define _BSP_F429_LED_H_

#include "main.h"
#include "stdbool.h"

/*
STM32_HAL LED XuanLib
2022/07/02
changelog：
1.0：实现F429板载LED控制
*/

//LED颜色定义

#define LED_COLOR_RED   (bool)1
#define LED_COLOR_GREEN (bool)0

//LED状态定义

#define LED_ON  (bool)1
#define LED_OFF (bool)0

void F429_LED(bool LED_Color, bool LED_Status);  //控制F429开发板板载LED
void F429_LED_Toggle(bool LED_Color);  //翻转F429开发板板载LED

#endif
