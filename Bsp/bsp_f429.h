#ifndef _BSP_F429_H_
#define _BSP_F429_H_

#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "cmsis_os.h"

#include "bsp_at24cxx.h"
#include "bsp_w25qxx.h"

/*
STM32_HAL F411 XuanLib
2022/07/02
changelog：
1.0：实现F429开发板板载资源测试
*/

//F429开发板测试函数

void F429_Test(void);
void F429_Test_AT24C02(void);
void F429_Test_W25Q256(void);

#endif
