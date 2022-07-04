#ifndef _BSP_W25QXX_H_
#define _BSP_W25QXX_H_

#include "main.h"
#include "spi.h"

/*
STM32_HAL W25QXX XuanLib
2022/07/02
changelog：
1.0：实现SPI Flash：W25Q256的读写
*/

/*
SPI5:
GPIOF.PIN6 = SPI5_CS
GPIOF.PIN7 = SPI5_SCK
GPIOF.PIN8 = SPI5_MISO
GPIOF.PIN9 = SPI5_MOSI
CubeMX:
Clock Polarity = High    //串行同步时钟的空闲状态为高电平
Clock Phase    = 2 Edge  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
*/

//使用FreeRTOS功能
#define FreeRTOS

uint8_t SPI5_ReadWriteByte(uint8_t TxData);  //SPI5操作函数

//W25QXX系列芯片ID
#define W25Q08  0XEF13
#define W25Q16  0XEF14
#define W25Q32  0XEF15
#define W25Q64  0XEF16
#define W25Q128 0XEF17
#define W25Q256 0XEF18

extern uint16_t W25QXX_TYPE;

//W25QXX指令集
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg1		0x05
#define W25X_ReadStatusReg2		0x35
#define W25X_ReadStatusReg3		0x15
#define W25X_WriteStatusReg1    0x01
#define W25X_WriteStatusReg2    0x31
#define W25X_WriteStatusReg3    0x11
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_Sleep			    0xB9
#define W25X_WakeUp	            0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

//W25QXX片选信号函数：

void W25QXX_CS(GPIO_PinState Status);

//W25QXX功能函数：

void W25QXX_Init(void);  //初始化W25QXX
uint16_t W25QXX_ReadID(void);  //读取FLASH ID
uint8_t W25QXX_ReadSR(uint8_t Regno);  //读取状态寄存器

void W25QXX_Write_SR(uint8_t Regno, uint8_t Sr);  //写状态寄存器
void W25QXX_Write_Enable(void);  //写使能 
void W25QXX_Write_Disable(void);  //写保护
void W25QXX_Write_NoCheck(uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite);  //无校验写入
void W25QXX_Write(uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite);  //写入flash
void W25QXX_Write_Page(uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite);  //写入flash(页)
void W25QXX_Read(uint32_t ReadAddr, uint8_t* pBuffer, uint16_t NumByteToRead);  //读取flash

void W25QXX_Erase_Chip(void);  //整片擦除
void W25QXX_Erase_Sector(uint32_t Dst_Addr);  //扇区擦除
void W25QXX_Wait_Busy(void);  //等待空闲

void W25QXX_Sleep(void);  //进入掉电模式
void W25QXX_WakeUp(void);  //唤醒

#ifdef FreeRTOS
#include "cmsis_os.h"
#define YMX_Delay osDelay
#else  //不使用FreeRTOS
#include "bsp_delay.h"
#define YMX_Delay Delay_ms
#endif

#endif
