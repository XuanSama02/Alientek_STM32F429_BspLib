#ifndef _BSP_MALLOC_H_
#define _BSP_MALLOC_H_

#include "main.h"

/*
STM32_HAL Malloc XuanLib
2022/05/31
changelog：
1.0:实现对内存的管理算法
*/

//定义三个内存池

#define SRAM_IN  0  //内部内存池
#define SRAM_EX  1  //外部内存池
#define SRAM_CCM 2  //CCM内存池(此部分仅CPU能访问)

#define SRAM_BANK 3  //支持的SDRAM块数

//Mem1内存参数设定，Mem1处于内部SRAM
#define MEM1_BLOCK_SIZE        64                             //内存块大小：64Btyes
#define MEM1_MAX_SIZE          160*1024                       //最大管理内存：160KBytes
#define MEM1_ALLOC_TABLE_SIZE  MEM1_MAX_SIZE/MEM1_BLOCK_SIZE  //内存表大小：自动计算

//Mem2内存参数设定，Mem2处于外部SDRAM
#define MEM2_BLOCK_SIZE        64                             //内存块大小：64Btyes
#define MEM2_MAX_SIZE          28912*1024                     //最大管理内存：28912KBytes
#define MEM2_ALLOC_TABLE_SIZE  MEM2_MAX_SIZE/MEM2_BLOCK_SIZE  //内存表大小：自动计算

//Mem3内存参数设定，Mem3处于CCM(仅CPU能访问)
#define MEM3_BLOCK_SIZE        64                             //内存块大小：64Btyes
#define MEM3_MAX_SIZE          60*1024                        //最大管理内存：60KBytes
#define MEM3_ALLOC_TABLE_SIZE  MEM3_MAX_SIZE/MEM3_BLOCK_SIZE  //内存表大小：自动计算

struct Malloc_Manager
{
    void (*Init)(uint8_t);          //初始化
    uint16_t (*PerUserd)(uint8_t);  //内存使用率
    uint8_t *MemBase[SRAM_BANK];    //内存池(管理SRAM_BANK个区域的内存)
    uint32_t *MemMap[SRAM_BANK];    //内存管理类状态表
    uint8_t MemReady[SRAM_BANK];    //内存管理是否就绪
}

extern YMX_Malloc;

void Malloc_Mem_Set(void *Mem_Addr, uint8_t Mem_Data, uint32_t Mem_Size);  //设置内存(仅内部调用)
void Malloc_Mem_Copy(void *Mem_Des, void *Mem_Src, uint32_t Mem_Size);     //复制内存(仅内部调用)
void Malloc_Mem_Init(uint8_t SRAM_Target);                                  //内存管理初始化函数(内/外部调用)
uint32_t Malloc_Mem_Malloc(uint8_t SRAM_Target, uint32_t Mem_Size);         //内存分配(仅内部调用)
uint8_t Malloc_Mem_Free(uint8_t SRAM_Target, uint32_t Mem_Offset);          //内存释放(仅内部调用)
uint16_t Malloc_Mem_PerUsed(uint8_t SRAM_Target);                           //获得内存使用率(内/外部调用)

//用户使用函数

void Malloc_Free(uint8_t SRAM_Target, void* Mem_Ptr);                     //内存释放(外部调用)
void *Malloc_Malloc(uint8_t SRAM_Target, uint32_t Mem_Size);              //内存分配(外部调用)
void *Malloc_Realloc(uint8_t SRAM_Target, void* Mem_Ptr, uint32_t Mem_Size);  //重新分配内存(外部调用)

#endif
