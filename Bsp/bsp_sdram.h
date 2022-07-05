#ifndef _BSP_SDRAM_H_
#define _BSP_SDRAM_H_

#include "main.h"
#include "stdbool.h"
#include "bsp_delay_tim.h"
#include "fmc.h"

/*
STM32_HAL SDRAM XuanLib
2022/07/05
changelog：
1.实现SDRAM的控制
*/

#define Bank5_SDRAM_ADDR ((uint32_t)(0xC0000000))  //SDRAM起始地址

//SDRAM配置参数(寄存器)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

extern SDRAM_HandleTypeDef hsdram1;  //SDRAM句柄

void SDRAM_Init(void);

HAL_StatusTypeDef SDRAM_Send_Cmd(bool Bankx, uint8_t Cmd, uint8_t Refresh, uint16_t Regval);
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t nBytes);
void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t nBytes);

#endif
