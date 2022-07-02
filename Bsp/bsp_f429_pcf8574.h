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

//I2C地址
#define I2C_ADDR     0x80

//PCF8574 I2C地址
#define PCF8574_ADDR 0x40

//PCF8574各个IO的功能
#define BEEP_IO      0  //蜂鸣器使能引脚
#define AP_INT_IO    1  //AP3216C中断引脚
#define DCMI_PWDN_IO 2  //DCMI电源控制引脚
#define USB_PWR_IO   3  //USB电源控制引脚
#define EX_IO        4  //拓展IO(自定义使用)
#define MPU_INT_IO   5  //MPU9250中断引脚
#define RS485_RE_IO  6  //RS485_RE引脚
#define ETH_RESET_IO 7  //以太网复位引脚

HAL_StatusTypeDef PCF8574_Init(void);  //初始化PCF8574

bool PCF8574_ReadBit(uint8_t Bit);  //读取位
void PCD8574_WriteBit(uint8_t Bit, bool Status);  //写指定位

uint8_t PCF8574_ReadByte(void);  //读取字节
void PCF8574_WriteByte(uint8_t Data);  //写字节

#endif
