#include "bsp_at24cxx.h"

uint16_t EEPROM_TYPE = 0;

/**
 * @brief 初始化AT24CXX
 * 
 * @param EEPROM_TYPE AT24CXX具体型号
 */
void AT24CXX_Init(uint16_t AT24CXX)
{
    EEPROM_TYPE = AT24CXX;  //初始化EEPROM_TYPE
}

/**
 * @brief 检查AT24CXX是否正常，地址末最后一位存储标志位
 * 
 * @return true 检测成功
 * @return false 检测失败
 */
bool AT24CXX_Check(void)
{
    uint8_t Check_Flag = 0;
    AT24CXX_BufferRead(&Check_Flag, 255, 1);  //读取标志位
    if(Check_Flag == 0x55)  //检查标志位是否为0x55
        return true;
    else  //排除为第一次初始化的情况
    {
        uint8_t Check_Flag_Init = 0x55;
        AT24CXX_ByteWrite(&Check_Flag_Init, 255);  //标志位写入0x55
        Check_Flag = AT24CXX_BufferRead(&Check_Flag, 255, 1);
        if(Check_Flag == 0x55)
            return true;
    }
    return false;  //失败
}

/**
 * @brief EEPROM读取数据到缓存区
 * 
 * @param pBuffer 数据缓存区指针
 * @param ReadAddr 读取地址
 * @param NumByteToRead 读取的字节数
 * @return uint32_t HAL_OK
 */
uint32_t AT24CXX_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    return HAL_I2C_Mem_Read(&hi2c2, EEPROM_ADDR, ReadAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)pBuffer, NumByteToRead, 1000);
}

/**
 * @brief EEPROM缓存区数据写入
 * 
 * @param pBuffer 数据缓存区指针
 * @param WriteAddr 写入地址
 * @param NumByteToWrite 写入的字节数
 */
void AT24CXX_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
    //计算写入数据
    uint8_t Addr = 0;
    uint8_t nCount = 0;
    uint8_t Num_Page = 0;
    uint8_t Num_Single = 0;
    Addr = WriteAddr % EEPROM_PAGESIZE;
    nCount = EEPROM_PAGESIZE - Addr;
    Num_Page = NumByteToWrite / EEPROM_PAGESIZE;
    Num_Single = NumByteToWrite % EEPROM_PAGESIZE;
    //开始写入
    if(Addr == 0)  //如果写入地址与EEPROM_PAGESIZE对齐
    {
        if(Num_Page == 0)  //如果NumByteToWrite < EEPROM_PAGESIZE
            AT24CXX_PageWrite(pBuffer, WriteAddr, Num_Single);
        else  //如果NumByteToWrite > EEPROM_PAGESIZE
        {
            while(Num_Page--)
            {
                AT24CXX_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE);
                WriteAddr += EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }
            if(Num_Single != 0)
                AT24CXX_PageWrite(pBuffer, WriteAddr, Num_Single);
        }
    }
    else  //如果写入地址与EEPROM_PAGESIZE未对齐
    {
        if(Num_Page == 0)  //如果NumByteToWrite < EEPROM_PAGESIZE
            AT24CXX_PageWrite(pBuffer, WriteAddr, Num_Single);
        else  //如果NumByteToWrite > EEPROM_PAGESIZE
        {
            NumByteToWrite -= nCount;
            Num_Page = NumByteToWrite / EEPROM_PAGESIZE;
            Num_Single = NumByteToWrite % EEPROM_PAGESIZE;
            if(nCount != 0)
            {
                AT24CXX_PageWrite(pBuffer, WriteAddr, nCount);
                WriteAddr += nCount;
                pBuffer += nCount;
            }
            while(Num_Page--)
            {
                AT24CXX_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE);
                WriteAddr += EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }
            if(Num_Single != 0)
                AT24CXX_PageWrite(pBuffer, WriteAddr, Num_Single);
        }
    }
}

/**
 * @brief 写入一个字节到EEPROM
 * 
 * @param pBuffer 数据缓存区指针
 * @param WriteAddr 写入地址
 * @return uint32_t HAL_OK
 */
uint32_t AT24CXX_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr)
{
    HAL_StatusTypeDef I2C_Status = HAL_OK;
    I2C_Status = HAL_I2C_Mem_Write(&hi2c2, EEPROM_ADDR, (uint16_t)WriteAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, 1, 100);
    if(I2C_Status != HAL_OK)
        printf("AT24CXX Data Transmit Failed!\r\n");
    while(HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
        osDelay(1);
    while(HAL_I2C_IsDeviceReady(&hi2c2, EEPROM_ADDR, EEPROM_MAX_TRIALS, EEPROM_TIMEOUT_MAX))
        osDelay(1);
    while(HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
        osDelay(1);
    return I2C_Status;
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param ReadAddr 
 * @param NumByteToWrite 
 * @return uint32_t 
 */
uint32_t AT24CXX_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
    HAL_StatusTypeDef I2C_Status = HAL_OK;
    I2C_Status = HAL_I2C_Mem_Write(&hi2c2, EEPROM_ADDR, WriteAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)pBuffer, NumByteToWrite, 100);
    if(I2C_Status != HAL_OK)
        printf("AT24CXX Data Transmit Failed!\r\n");
    while(HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
        osDelay(1);
    while(HAL_I2C_IsDeviceReady(&hi2c2, EEPROM_ADDR, EEPROM_MAX_TRIALS, EEPROM_TIMEOUT_MAX))
        osDelay(1);
    while(HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
        osDelay(1);
    return I2C_Status;
}
