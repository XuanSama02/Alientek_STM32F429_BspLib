#include "bsp_sdram.h"

/**
 * @brief SDRAM初始化函数
 * 
 */
void SDRAM_Init(void)
{
    uint32_t temp = 0;
    //SDRAM初始化:
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);        //使能时钟
    osDelay(1);                                               //延迟>200us
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);              //对所有存储区预充电
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);  //设置自刷新次数
    /*配置模式寄存器：
    bit0~bit2  ：指定突发访问长度
    bit3       ：指定突发访问类型
    bit4~bit6  ：CAS值
    bit7~bit8  ：运行模式
    bit9       ：指定写突发模式
    bit10~bit11：保留位
    */
    temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |  //设置突发长度：1(1/2/4/8)
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |  //设置突发类型：连续(连续/交错)
                     SDRAM_MODEREG_CAS_LATENCY_3           |  //设置CAS值：3(1/2/3)
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |  //设置操作模式：0，标准模式
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;    //设置突发写模式：1，单点访问
    SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);   //设置SDRAM的模式寄存器
}

/**
 * @brief SDRAM指令函数
 * 
 * @param Bankx 指令目标(0：BANK5，1：BANK6)
 * @param Cmd 命令(0：正常模式/1：时钟配置使能/2：预充电存储区/3：自动刷新/4：加载模式寄存器/5：自刷新/6：掉电模式)
 * @param Refresh 自刷新次数
 * @param Regval 模式寄存器值
 * @return HAL_StatusTypeDef HAL_OK
 */
HAL_StatusTypeDef SDRAM_Send_Cmd(bool Bankx, uint8_t Cmd, uint8_t Refresh, uint16_t Regval)
{
    
    //选择目标Bank
    uint32_t Bankx_Target = 0;
    if(Bankx == 0)
        Bankx_Target = FMC_SDRAM_CMD_TARGET_BANK1;
    else
        Bankx_Target = FMC_SDRAM_CMD_TARGET_BANK2;
    //配置指令结构体
    FMC_SDRAM_CommandTypeDef SDRAM_Cmd;
    SDRAM_Cmd.CommandMode = Cmd;                //命令
    SDRAM_Cmd.CommandTarget = Bankx_Target;     //目标Bank
    SDRAM_Cmd.AutoRefreshNumber = Refresh;      //自刷新次数
    SDRAM_Cmd.ModeRegisterDefinition = Regval;  //模式寄存器值
    return HAL_SDRAM_SendCommand(&hsdram1, &SDRAM_Cmd, 0x1000);
}

/**
 * @brief 往指定地址(WriteAddr+Bank5_SDRAM_ADDR)开始，连续写入nBytes个字节
 * 
 * @param pBuffer 字节指针
 * @param WriteAddr 写入地址
 * @param nBtyes 写入长度
 */
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t nBytes)
{
    for(; nBytes!=0; nBytes--)
    {
        *(volatile uint8_t*)(Bank5_SDRAM_ADDR + WriteAddr) = *pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

/**
 * @brief 往指定地址(WriteAddr+Bank5_SDRAM_ADDR)开始，连续读取nBytes个字节
 * 
 * @param pBuffer 字节指针
 * @param ReadAddr 读取地址
 * @param nBytes 读取长度
 */
void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t nBytes)
{
    for(; nBytes!=0; nBytes--)
    {
        *pBuffer++ = *(volatile uint8_t*)(Bank5_SDRAM_ADDR + ReadAddr);
        ReadAddr++;
    }
}
