#ifndef _BSP_F429_PCF8574_H_
#define _BSP_F429_PCF8574_H_

#include "main.h"
#include "stdbool.h"

/*
STM32_HAL PCF8574 XuanLib
2022/07/02
changelog：
1.0：实现F429板载PCF8574芯片的控制
*/

//IO端口所有接口为低电平有效果
#define IO_ENABLE  false
#define IO_DISABLE true

//PCF8574地址
#define PCF8574_ADDR  0x40

//PCF8574各个IO的功能
#define IO_BEEP      0  //蜂鸣器使能引脚
#define IO_AP_INT    1  //AP3216C中断引脚
#define IO_DCMI_PWDN 2  //DCMI电源控制引脚
#define IO_USB_PWR   3  //USB电源控制引脚
#define IO_EX        4  //拓展IO(自定义使用)
#define IO_MPU_INT   5  //MPU9250中断引脚
#define IO_RS485_RE  6  //RS485_RE引脚
#define IO_ETH_RESET 7  //以太网复位引脚

//PCF8574功能函数

HAL_StatusTypeDef PCF8574_Init(void);  //初始化PCF8574
void PCF8574_INT(bool IO_INT_Status);  //控制INT引脚

bool PCF8574_ReadBit(uint8_t IO_Bit);  //读取位
HAL_StatusTypeDef PCF8574_WriteBit(uint8_t IO_Bit, bool IO_Status);  //写指定位

uint8_t PCF8574_ReadByte(void);  //读取字节
HAL_StatusTypeDef PCF8574_WriteByte(uint8_t IO_Data);  //写字节

#endif
