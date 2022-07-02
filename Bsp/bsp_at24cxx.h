#ifndef _BSP_AT24CXX_H_
#define _BSP_AT24CXX_H_

#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include "i2c.h"

/*
STM32_HAL AT24CXX XuanLib
2022/07/02
changelog：
1.0：实现AT24CXX的硬件I2C读取
*/

//AT24CXX ID
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767

//STM32F429开发板板载AT24C02
extern uint16_t EEPROM_TYPE;

//AT24C01，02，每页有8个字节
#define EEPROM_PAGESIZE 8
//AT24C04，04，16，每页有16个字节
//#define EEPROM_PAGESIZE 16

//AT24CXX设备地址(只需要与外挂I2C器件地址不同即可)
#define EEPROM_DEVICE_ADDR 0x0A

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
//AT24CXX地址
#define EEPROM_ADDR           0xA0

//AT24CXX Block地址
#define EEPROM_Block0_ADDRESS 0xA0 //E2 = 0
#define EEPROM_Block1_ADDRESS 0xA2 //E2 = 0
#define EEPROM_Block2_ADDRESS 0xA4 //E2 = 0
#define EEPROM_Block3_ADDRESS 0xA6 //E2 = 0

//AT24CXX I2C2中断
#define EEPROM_I2C_EV_IRQn      I2C2_EV_IRQn
#define EEPROM_I2C_ER_IRQn      I2C2_ER_IRQn
#define EEPROM_I2C_EV_IRQHander I2C2_EV_IRQHander
#define EEPROM_I2C_ER_IRQHander I2C2_ER_IRQHander

//EEPROM I2C2等待超时时间
#define EEPROM_I2C_FLAG_TIMEOUT ((uint32_t)0x1000)
#define EEPROM_I2C_LONG_TIMEOUT ((uint32_t)(10*EEPROM_I2C_FLAG_TIMEOUT))

//EEPROM I2C2时间(用于HAL_I2C_IsDeviceReady函数)
#define EEPROM_TIMEOUT_MAX 300
#define EEPROM_MAX_TRIALS  300

void AT24CXX_Init(uint16_t AT24CXX);  //初始化
bool AT24CXX_Check(void);  //检查器件

//AT24CXX读取

uint32_t AT24CXX_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

//AT24CXX写入

void AT24CXX_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
uint32_t AT24CXX_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr);
uint32_t AT24CXX_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);

#endif
