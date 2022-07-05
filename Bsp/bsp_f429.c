#include "bsp_f429.h"

/**
 * @brief F429开发板测试函数
 * 
 */
void F429_Test(void)
{
    printf("\r\n");
    printf("F429 Test Starting:\r\n");
    osDelay(2000);
    F429_Test_AT24C02();
    F429_Test_W25Q256();
    F429_Test_SDRAM();
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
    printf("F429_TEST: AT24C02\r\n");
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
    printf("F429_TEST: W25Q256\r\n");
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

/**
 * @brief F429开发板板载SDRAM测试
 * 
 */
/*
void F429_Test_SDRAM(void)
{
    ;
}
*/

/**
 * @brief F429开发板板载SDRAM测试(正点原子)
 * 
 */
void F429_Test_SDRAM(void)
{
    printf("F429_TEST: AT24C02\r\n");
    osDelay(1000);
    uint16_t TestSram[250000] __attribute__((at(0XC0000000)));  //SDRAM测试用数组
    uint32_t nCount = 0;
    uint32_t Temp = 0;
    uint32_t Sval = 0;  //在地址0读取到的数据
    //预存测试用的数据
    for(nCount=0; nCount<250000; nCount++)
        TestSram[nCount] = nCount;
    //每隔16KBytes，写入1个数据，总共写入2048个数据，刚好是32MBytes
    for(nCount=0; nCount<32*1024; nCount+=16*1024)
    {
        *(volatile uint32_t*)(Bank5_SDRAM_ADDR+nCount) = Temp;
        Temp++;
    }
    //依次读出之前写入的数据(数据读出校验)
    for(nCount=0; nCount<32*1024*1024; nCount+=16*1024)
    {
        Temp = *(volatile uint32_t*)(Bank5_SDRAM_ADDR+nCount);
        if(nCount == 0)
            Sval = Temp;
        else if(Temp <= Sval)  //校验原理：后读出的数据>先读出的数据
            printf("SDRAM Test Failed! Position: %d", Temp);             //测试失败
        printf("SDRAM Capacity: %dKB\r\n", (uint16_t)(Temp-Sval+1)*16);  //打印SDRAM容量
    }
    osDelay(2000);
}

