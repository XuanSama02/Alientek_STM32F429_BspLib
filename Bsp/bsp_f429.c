#include "bsp_f429.h"

/**
 * @brief F429开发板测试函数
 * 
 */
void F429_Test(void)
{
    printf("\r\n");
    printf("F411 Test Starting:\r\n");
    F429_Test_AT24C02();
    F429_Test_W25Q256();
}

/**
 * @brief F429开发板板载EEPROM测试
 * 
 */
void F429_Test_AT24C02(void)
{
    //初始化测试变量
    uint8_t TEST[] = {"AT24C02 TEST Success!"};
    uint8_t Read[sizeof(TEST)];
    printf("F411_TEST: AT24C02\r\n");
    osDelay(1000);
    //初始化硬件
    AT24CXX_Init(AT24C02);
    if(!AT24CXX_Check())
        printf("AT24C02 Init Failed!\r\n");
    else
        printf("AT24C02 Init Success!\r\n");
    osDelay(1000);
    //运行测试
    AT24CXX_BufferWrite((uint8_t*)TEST, 0, sizeof(TEST));
    AT24CXX_BufferRead(Read, 0, sizeof(TEST));
    printf("%s\r\n", Read);
    printf("\r\n");
    osDelay(2000);
}

/**
 * @brief F429开发板板载SPI Flash测试
 * 
 */
void F429_Test_W25Q256(void)
{
    //初始化测试变量
    uint8_t TEST[] = {"W25Q256 TEST Success!"};
    uint8_t Read[sizeof(TEST)];
    uint16_t ID = W25QXX_ReadID();
    printf("F411_TEST: W25Q256\r\n");
    osDelay(1000);
    //初始化硬件
    W25QXX_Init();
    if((ID==0) || (ID==0xFFFF))
        printf("Read ID Failed!\r\n");
    else
        printf("Flash ID: %#x\r\n", ID);
    osDelay(1000);
    //运行测试
    W25QXX_Write((2*1024*1024)-100, (uint8_t*)TEST, sizeof(TEST));
    W25QXX_Read((2*1024*1024)-100, Read, sizeof(TEST));
    printf("%s\r\n", Read);
    printf("\r\n");
    osDelay(2000);
}
