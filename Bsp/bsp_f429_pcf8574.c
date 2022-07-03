#include "bsp_f429_pcf8574.h"

/**
 * @brief 初始化PCF8574
 * 
 * @return HAL_StatusTypeDef 初始化状态
 */
HAL_StatusTypeDef PCF8574_Init(void)
{
    HAL_GPIO_WritePin(PCF8574_INT_GPIO_Port, PCF8574_INT_Pin, GPIO_PIN_SET);
    PCF8574_WriteByte(0xFF);  //默认所有IO输出高电平
    if(PCF8574_ReadByte() == 0xFF)
        return HAL_OK;
    else
        return HAL_ERROR;
}

/**
 * @brief 控制INT引脚
 * 
 * @param IO_INT_Status INT引脚状态
 */
void PCF8574_INT(bool IO_INT_Status)
{
    if(IO_INT_Status == IO_ENABLE)
        HAL_GPIO_WritePin(PCF8574_INT_GPIO_Port, PCF8574_INT_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(PCF8574_INT_GPIO_Port, PCF8574_INT_Pin, GPIO_PIN_RESET);
}

/**
 * @brief 读取PCF8574某个IO的状态
 * 
 * @param IO_Bit IO编号(0-7)
 * @return true 高电平
 * @return false 低电平
 */
bool PCF8574_ReadBit(uint8_t IO_Bit)
{
    uint8_t IO_Data = 0x00;
    IO_Data = PCF8574_ReadByte();  //获取当前的8位IO值
    if(IO_Data&(1<<IO_Bit))
        return true;
    return false;
}

/**
 * @brief 写入PCF8574某个IO的状态
 * 
 * @param IO_Bit IO编号(0-7)
 * @param IO_Status IO状态
 * @return HAL_StatusTypeDef 写入状态
 */
HAL_StatusTypeDef PCF8574_WriteBit(uint8_t IO_Bit, bool IO_Status)
{
    uint8_t IO_Data = 0x00;
    IO_Data = PCF8574_ReadByte();  //获取当前的8位IO值
    if(IO_Status == false)
        IO_Data &= ~(1<<IO_Bit);
    else
        IO_Data |= (1<<IO_Bit);
    return PCF8574_WriteByte(IO_Data);  //写入新的8位IO值
}

/**
 * @brief 读取PCF8574的8位IO值
 * 
 * @return uint8_t 8位IO值
 */
uint8_t PCF8574_ReadByte(void)
{
    uint8_t PCF8574_Status = 0xFF;
    if(HAL_I2C_Master_Receive(&hi2c2, (PCF8574_ADDR|0x01), &PCF8574_Status, sizeof(PCF8574_Status), 100) != HAL_OK)
        printf("PCF8574 Read Failed!\r\n");
    return PCF8574_Status;
}

/**
 * @brief 写入PCF8574的8位IO值
 * 
 * @param IO_Data 8位IO值
 * @return HAL_StatusTypeDef 写入状态
 */
HAL_StatusTypeDef PCF8574_WriteByte(uint8_t IO_Data)
{
    return HAL_I2C_Master_Transmit(&hi2c2, (PCF8574_ADDR|0x00), &IO_Data, sizeof(IO_Data), 100);
}
