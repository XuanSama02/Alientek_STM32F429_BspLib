#include "bsp_malloc.h"

//内存池(32Bytes对齐)

__align(32) uint8_t Mem1Base[MEM1_MAX_SIZE];                                  //内部SRAM内存池
__align(32) uint8_t Mem2Base[MEM2_MAX_SIZE] __attribute__((at(0xC01F4000)));  //外部SDRAM内存池
__align(32) uint8_t Mem3Base[MEM3_MAX_SIZE] __attribute__((at(0x10000000)));  //内部CCM内存池

//内存管理表

uint32_t Mem1MapBase[MEM1_ALLOC_TABLE_SIZE];                                                  //内部SRAM内存池MAP
uint32_t Mem2MapBase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0XC01F4000 + MEM2_MAX_SIZE)));  //外部SDRAM内存池MAP
uint32_t Mem3MapBase[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(0X10000000 + MEM3_MAX_SIZE)));  //内部CCM内存池MAP

//内存管理参数

const uint32_t MemTableSize[SRAM_BANK] = {MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE, MEM3_ALLOC_TABLE_SIZE};  //内存表大小
const uint32_t MemBlockSize[SRAM_BANK] = {MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE, MEM3_BLOCK_SIZE};                     //内存分块大小
const uint32_t MemSize[SRAM_BANK] = {MEM1_MAX_SIZE, MEM2_MAX_SIZE, MEM3_MAX_SIZE};                               //内存总大小

struct Malloc_Manager YMX_Malloc =
{
    Malloc_Mem_Init,                        //内存初始化
    Malloc_Mem_PerUsed,                     //内存使用率
    Mem1Base, Mem2Base, Mem3Base,           //内存池
    Mem1MapBase, Mem2MapBase, Mem3MapBase,  //内存管理状态表
    false, false, false                     //内存管理未就绪
};

/**
 * @brief 设置内存(仅内部调用)
 * 
 * @param Mem_Addr 内存首地址
 * @param Mem_Data 内存值
 * @param Mem_Size 内存大小(Bytes)
 */
void Malloc_Mem_Set(void *Mem_Addr, uint8_t Mem_Data, uint32_t Mem_Size)
{
    uint8_t *Mem_Addr_Copy = Mem_Addr;
    while(Mem_Size--)*Mem_Addr_Copy++ = Mem_Data;
}

/**
 * @brief 复制内存(仅内部调用)
 * 
 * @param Mem_Des 内存目的地址
 * @param Mem_Src 内存源地址
 * @param Men_Size 复制内存长度(Bytes)
 */
void Malloc_Mem_Copy(void *Mem_Des, void *Mem_Src, uint32_t Mem_Size)
{
    uint8_t *Mem_Des_Copy = Mem_Des;
    uint8_t *Mem_Src_Copy = Mem_Src;
    while(Mem_Size--)*Mem_Des_Copy++=*Mem_Src_Copy++;
}

/**
 * @brief 内存管理初始化函数(内/外部调用)
 * 
 * @param SRAM_Target 目标内存块
 */
void Malloc_Mem_Init(uint8_t SRAM_Target)
{
    Malloc_Mem_Set(YMX_Malloc.MemMap[SRAM_Target], 0, MemTableSize[SRAM_Target]*4);  //内存状态表数据清零
    YMX_Malloc.MemReady[SRAM_Target] = true;                                            //内存管理初始化标志位置1(初始化完成)
}

/**
 * @brief 内存分配(仅内部调用)
 * 
 * @param SRAM_Target 目标内存块
 * @param Mem_Size 分配内存大小
 * @return uint32_t 内存偏移地址(0xFFFFFFFF代表分配失败)
 */
uint32_t Malloc_Mem_Malloc(uint8_t SRAM_Target, uint32_t Mem_Size)
{
    //设置变量
    signed long Mem_Offset = 0;
    uint32_t Mem_Blocks_Need = 0;   //需要的内存块数
    uint32_t Mem_Blocks_Empty = 0;  //连续空内存块数
    uint32_t nCount = 0;
    //初始化判断与传入值检查
    if(!YMX_Malloc.MemReady[SRAM_Target])  //未初始化，先执行初始化
        YMX_Malloc.Init(SRAM_Target);
    if(Mem_Size == 0)  //分配大小=0(不需要分配或传入值错误)
        return 0xFFFFFFFF;
    //计算内存分配
    Mem_Blocks_Need = Mem_Size/MemBlockSize[SRAM_Target];  //获取需要连续分配的内存数
    if(Mem_Size % MemBlockSize[SRAM_Target])
        Mem_Blocks_Need++;
    //开始内存分配
    for(Mem_Offset = MemBlockSize[SRAM_Target]-1; Mem_Offset>=0; Mem_Offset--)  //搜索整个内存控制区
    {
        //遍历寻找连续空内存块
        if(!YMX_Malloc.MemMap[SRAM_Target][Mem_Offset])  //发现空内存块，连续空内存块数++
            Mem_Blocks_Empty++;
        else                                             //发现非空内存块，连续空内存块数清零
            Mem_Blocks_Empty = 0;
        //成功找到连续Mem_Blocks_Need个空内存块
        if(Mem_Blocks_Empty == Mem_Blocks_Need)
        {
            for(; nCount<Mem_Blocks_Need; nCount++)      //标注内存块非空
                YMX_Malloc.MemMap[SRAM_Target][Mem_Offset+nCount] = Mem_Blocks_Need;
            return (Mem_Offset*MemBlockSize[SRAM_Target]);  //返回偏移地址
        }
    }
    return 0xFFFFFFFF;  //分配失败(未找到足够连续空内存块)
}

/**
 * @brief 内存释放(仅内部调用)
 * 
 * @param SRAM_Target 目标内存块
 * @param Mem_Offset 内存偏移地址
 * @return uint8_t 错误码(0：释放成功，1：未初始化，2：偏移超区域)
 */
uint8_t Malloc_Mem_Free(uint8_t SRAM_Target, uint32_t Mem_Offset)
{
    uint32_t nCount = 0;
    if(!YMX_Malloc.MemReady[SRAM_Target])  //未初始化，先执行初始化
    {
        YMX_Malloc.Init(SRAM_Target);
        return 1;  //错误码1：未初始化
    }
    if(Mem_Offset < MemSize[SRAM_Target])  //偏移在内存池内(偏移未超区域)
    {
        uint32_t Mem_Blocks_Index = Mem_Offset/MemBlockSize[SRAM_Target];             //偏移所在内存块编号
        uint32_t Mem_Blocks_Need = YMX_Malloc.MemMap[SRAM_Target][Mem_Blocks_Index];  //内存块数量
        for(; nCount<Mem_Blocks_Need; nCount++)                                       //内存块清零
            YMX_Malloc.MemMap[SRAM_Target][Mem_Blocks_Index+nCount] = 0;
        return 0;  //成功码0：释放成功
    }
    else
        return 2;  //错误码2：偏移超区域
}

/**
 * @brief 获得内存使用率(内/外部调用)
 * 
 * @param SRAM_Target 目标内存块
 * @return uint16_t 使用率(扩大了10倍，0.0%~100.0%)
 */
uint16_t Malloc_Mem_PerUsed(uint8_t SRAM_Target)
{
    uint32_t Mem_Used = 0;
    uint32_t nCount = 0;
    for(; nCount<MemTableSize[SRAM_Target]; nCount++)  //遍历整个内存管理区域
    {
        if(YMX_Malloc.MemMap[SRAM_Target][nCount])     //内存数据非0(非空)
            Mem_Used++;
    }
    return (Mem_Used*1000)/(MemTableSize[SRAM_Target]);
}

/**
 * @brief 内存释放(外部调用)
 * 
 * @param SRAM_Target 目标内存块
 * @param Mem_Ptr 内存首地址
 */
void Malloc_Free(uint8_t SRAM_Target, void* Mem_Ptr)
{
    uint32_t Mem_Offset = 0;
    if(Mem_Ptr == NULL)  //地址为0
        return;
    Mem_Offset = (uint32_t)Mem_Ptr - (uint32_t)YMX_Malloc.MemBase[SRAM_Target];
    Malloc_Mem_Free(SRAM_Target, Mem_Offset);  //释放内存
}

/**
 * @brief 内存分配(外部调用)
 * 
 * @param SRAM_Target 目标内存块
 * @param Mem_Size 内存大小(Bytes)
 * @return void* 分配内存首地址(NULL分配失败)
 */
void *Malloc_Malloc(uint8_t SRAM_Target, uint32_t Mem_Size)
{
    uint32_t Mem_Offset = 0;
    Mem_Offset = Malloc_Mem_Malloc(SRAM_Target, Mem_Size);
    if(Mem_Offset == 0xFFFFFFFF)  //分配失败
        return NULL;
    else
        return (void*)((uint32_t)YMX_Malloc.MemBase[SRAM_Target]+Mem_Offset);  //返回内存首地址
}

/**
 * @brief 重新分配内存(外部调用)
 * 
 * @param SRAM_Target 目标内存块
 * @param Mem_Ptr 旧内存首地址
 * @param Mem_Size 分配内存大小
 * @return void* 新分配内存首地址
 */
void *Malloc_Realloc(uint8_t SRAM_Target, void* Mem_Ptr, uint32_t Mem_Size)
{
    uint32_t Mem_Offset = 0;
    Mem_Offset = Malloc_Mem_Malloc(SRAM_Target, Mem_Size);
    if(Mem_Offset == 0xFFFFFFFF)
        return NULL;
    else
    {
        Malloc_Mem_Copy((void*)((uint32_t)YMX_Malloc.MemBase[SRAM_Target]+Mem_Offset), Mem_Ptr, Mem_Size);  //拷贝旧内存内容到新内存
        Malloc_Free(SRAM_Target, Mem_Ptr);                                                                  //释放旧内存空间
        return (void*)((uint32_t)YMX_Malloc.MemBase[SRAM_Target]+Mem_Offset);                               //返回新内存地址
    }
}
