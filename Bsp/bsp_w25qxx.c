#include "bsp_w25qxx.h"

uint8_t W25QXX_Buffer[4096];  //W25QXX缓存
uint16_t W25QXX_TYPE = 0x00;  //Flash型号

/**
 * @brief 操作W25QXX所在SPI2片选信号
 * 
 * @param Status 
 */
void W25QXX_CS(GPIO_PinState Status)
{
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, Status);
}

/**
 * @brief SPI5读写函数
 * 
 * @param TxData 发送的数据
 * @return uint8_t 读出的数据
 */
uint8_t SPI5_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData = 0;
    HAL_SPI_TransmitReceive(&hspi5, &TxData, &RxData, 1, 0xFF);
    return RxData;
}

/**
 * @brief 初始化W25QXX
 * 
 */
void W25QXX_Init(void)
{
    uint8_t temp_SR = 0;
    W25QXX_CS(GPIO_PIN_SET);  //SPI Flash不选中
    W25QXX_TYPE = W25QXX_ReadID();  //读取Flash ID
    if(W25QXX_TYPE == W25Q256)  //SPI Flash为W25Q256
    {
        temp_SR = W25QXX_ReadSR(3);  //读取状态寄存器3
        if((temp_SR&0x01) == 0)  //不是4字节模式，则设置4字节模式
        {
            W25QXX_CS(GPIO_PIN_RESET);  //SPI Flash选中
            SPI5_ReadWriteByte(W25X_Enable4ByteAddr);  //4字节地址模式指令
            W25QXX_CS(GPIO_PIN_SET);  //SPI Flash不选中
        }
    }
}

/**
 * @brief 读取W25QXX的设备ID
 * 
 * @return uint16_t 设备ID
 */
uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_ManufactDeviceID);  //读取设备ID
    SPI5_ReadWriteByte(0x00);
    SPI5_ReadWriteByte(0x00);
    SPI5_ReadWriteByte(0x00);
    Temp |= SPI5_ReadWriteByte(0xFF)<<8;
    Temp |= SPI5_ReadWriteByte(0xFF);
    W25QXX_CS(GPIO_PIN_SET);
    return Temp;
}

/**
 * @brief 读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
 * 
 *状态寄存器1：
 *BIT7  6   5   4   3   2   1   0
 *SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *SPR:默认0,状态寄存器保护位,配合WP使用
 *TB,BP2,BP1,BP0:FLASH区域写保护设置
 *WEL:写使能锁定
 *BUSY:忙标记位(1,忙;0,空闲)
 *默认:0x00

 *状态寄存器2：
 *BIT7  6   5   4   3   2   1   0
 *SUS   CMP LB3 LB2 LB1 (R) QE  SRP1

 *状态寄存器3：
 *BIT7      6    5    4   3   2   1   0
 *HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 * 
 * @param Regno 状态寄存器号(1-3)
 * @return uint8_t 状态寄存器值
 */
uint8_t W25QXX_ReadSR(uint8_t Regno)
{
    uint8_t Byte = 0;
    uint8_t Command = 0;
    switch(Regno)
    {
        case 1:Command = W25X_ReadStatusReg1;break;  //读取状态寄存器1指令
        case 2:Command = W25X_ReadStatusReg2;break;  //读取状态寄存器2指令
        case 3:Command = W25X_ReadStatusReg3;break;  //读取状态寄存器3指令
        default:Command = W25X_ReadStatusReg1;break;
    }
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(Command);  //发送指令
    Byte = SPI5_ReadWriteByte(0xFF);  //读取一个字节
    W25QXX_CS(GPIO_PIN_SET);
    return Byte;
}

/**
 * @brief 写W25QXX状态寄存器
 * 
 * @param Regno 状态寄存器号
 * @param Sr 状态寄存器值
 */
void W25QXX_Write_SR(uint8_t Regno, uint8_t Sr)
{
    uint8_t Command = 0;
    switch(Regno)
    {
        case 1:Command = W25X_WriteStatusReg1;break;
        case 2:Command = W25X_WriteStatusReg2;break;
        case 3:Command = W25X_WriteStatusReg3;break;
        default:Command = W25X_WriteStatusReg1;break;
    }
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(Command);  //发送写状态寄存器命令
    SPI5_ReadWriteByte(Sr);  //写入状态寄存器值
    W25QXX_CS(GPIO_PIN_SET);
}

/**
 * @brief W25QXX写入使能
 * 
 */
void W25QXX_Write_Enable(void)
{
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_WriteEnable);
    W25QXX_CS(GPIO_PIN_SET);
}

/**
 * @brief W25QXX写入保护
 * 
 */
void W25QXX_Write_Disable(void)
{
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_WriteDisable);
    W25QXX_CS(GPIO_PIN_SET);
}

/**
 * @brief 无校验写入W25QXX
 * 
 * @param WriteAddr 写入起始地址(24bit)
 * @param pBuffer 数据存储区
 * @param NumByteToWrite 写入字节数(Max=65535)
 */
void W25QXX_Write_NoCheck(uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite)
{
    uint16_t PageRemain = 0;
    PageRemain = 256 - WriteAddr%256;  //单页剩余的字节
    if(NumByteToWrite <= PageRemain)  //不大于256字节
        PageRemain = NumByteToWrite;
    while(1)
    {
        W25QXX_Write_Page(WriteAddr, pBuffer, PageRemain);
        if(NumByteToWrite == PageRemain)  //写入完成
            break;
        else  //NumByteToWrite > PageRemain
        {
            pBuffer += PageRemain;
            WriteAddr += PageRemain;

            NumByteToWrite -= PageRemain;  //减去已经写入了的字节
            if(NumByteToWrite > 256)  //一次可以写入256字节
                PageRemain = 256;
            else  //不足够256字节
                PageRemain = NumByteToWrite;
        }
    }
}

/**
 * @brief 写入W25QXX，在指定地址开始写入指定长度的数据
 * 
 * @param WriteAddr 开始写入的地址(12bit)
 * @param pBuffer 数据存储区
 * @param NumByteToWrite 要写入的字节数(Max=65535)
 */
void W25QXX_Write(uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite)
{
    uint32_t SectorPosition = 0;
    uint16_t SectorOffset = 0;
    uint16_t SectorRemain = 0;
    uint16_t nCount = 0;
    uint8_t* W25QXX_Buf = 0;
    W25QXX_Buf = W25QXX_Buffer;
    SectorPosition = WriteAddr/4096;  //扇区地址
    SectorOffset = WriteAddr%4096;  //扇区内偏移量
    SectorRemain = 4096 - SectorOffset;  //扇区剩余空间
    if(NumByteToWrite <= SectorRemain)
        SectorRemain = NumByteToWrite;
    while(1)
    {
        W25QXX_Read(SectorPosition*4096, W25QXX_Buf, 4096);  //读出整个扇区的内容
        for(nCount=0; nCount<SectorRemain; nCount++)  //校验数据
            if(W25QXX_Buf[SectorOffset+nCount] != 0xFF)  //需要擦除
                break;
        if(nCount < SectorRemain)  //需要擦除
        {
            W25QXX_Erase_Sector(SectorPosition);  //擦除整个扇区
            for(nCount=0; nCount<SectorRemain; nCount++)  //复制
                W25QXX_Buf[nCount+SectorOffset] = pBuffer[nCount];
            W25QXX_Write_NoCheck(SectorPosition*4096, W25QXX_Buf, 4096);  //写入整个扇区
        }
        else  //写入已经擦除了的扇区，直接写入扇区剩余空间
            W25QXX_Write_NoCheck(WriteAddr, pBuffer, SectorRemain);
        if(NumByteToWrite == SectorRemain)  //写入结束
            break;
        else  //写入未结束
        {
            SectorPosition++;  //扇区地址++
            SectorOffset = 0;  //写入偏移地址

            pBuffer += SectorRemain;  //指针偏移
            WriteAddr += SectorRemain;  //写地址偏移
            NumByteToWrite -= SectorRemain;  //字节数递减
            if(NumByteToWrite > 4096)  //下个扇区空间不足
                SectorRemain = 4096;
            else  //下个扇区可以写完
                SectorRemain = NumByteToWrite;
        }
    }
}

/**
 * @brief W25QXX写入页
 * 
 * @param WriteAddr 写入地址(24bit)
 * @param pBuffer 数据存储区
 * @param NumByteToWrite 写入字节数(Max=256)
 */
void W25QXX_Write_Page(uint32_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite)
{
    W25QXX_Write_Enable();
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_PageProgram);  //写页指令
    if(W25QXX_TYPE == W25Q256)  //如果型号为W25Q256，地址为4字节，要发送最高8位
        SPI5_ReadWriteByte((uint8_t)((WriteAddr)>>24));
    SPI5_ReadWriteByte((uint8_t)((WriteAddr)>>16));
    SPI5_ReadWriteByte((uint8_t)((WriteAddr)>>8));
    SPI5_ReadWriteByte((uint8_t)(WriteAddr));
    for(uint16_t nCount=0; nCount<NumByteToWrite; nCount++)  //循环写入
        SPI5_ReadWriteByte(pBuffer[nCount]);
    W25QXX_CS(GPIO_PIN_SET);
    W25QXX_Wait_Busy();
}

/**
 * @brief 读取W25QXX，在指定地址开始读取指定长度的数据
 * 
 * @param ReadAddr 开始读取的地址(24bit)
 * @param pBuffer 数据存储区
 * @param NumByteToRead 要读取的字节数(Max=65535)
 */
void W25QXX_Read(uint32_t ReadAddr, uint8_t* pBuffer, uint16_t NumByteToRead)
{
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_ReadData);  //读取数据指令
    if(W25QXX_TYPE == W25Q256)  //如果型号为W25Q256，地址为4字节，要发送最高8位
        SPI5_ReadWriteByte((uint8_t)((ReadAddr)>>24));
    SPI5_ReadWriteByte((uint8_t)((ReadAddr)>>16));
    SPI5_ReadWriteByte((uint8_t)((ReadAddr)>>8));
    SPI5_ReadWriteByte((uint8_t)(ReadAddr));
    for(uint16_t nCount=0; nCount<NumByteToRead; nCount++)  //循环读数据
        pBuffer[nCount] = SPI5_ReadWriteByte(0xFF);
    W25QXX_CS(GPIO_PIN_SET);
}

/**
 * @brief W25QXX全盘擦除，需要很长的时间
 * 
 */
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();  //写入使能
    W25QXX_Wait_Busy();
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_ChipErase);  //全盘擦除指令
    W25QXX_CS(GPIO_PIN_SET);
    W25QXX_Wait_Busy();
}

/**
 * @brief W25QXX扇区擦除
 * 
 * @param Dst_Addr 扇区地址，擦除一个扇区最小用时:150ms
 */
void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_SectorErase);
    if(W25QXX_TYPE == W25Q256)  //如果型号为W25Q256，地址为4字节，要发送最高8位
        SPI5_ReadWriteByte((uint8_t)((Dst_Addr)>>24));
    SPI5_ReadWriteByte((uint8_t)((Dst_Addr)>>16));
    SPI5_ReadWriteByte((uint8_t)((Dst_Addr)>>8));
    SPI5_ReadWriteByte((uint8_t)(Dst_Addr));
    W25QXX_CS(GPIO_PIN_SET);
    W25QXX_Wait_Busy();
}

/**
 * @brief W25QXX等待空闲
 * 
 */
void W25QXX_Wait_Busy(void)
{
    while((W25QXX_ReadSR(1) & 0x01) == 0x01);
        YMX_Delay(1);
}

/**
 * @brief W25QXX进入掉电模式
 * 
 */
void W25QXX_Sleep(void)
{
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_Sleep);  //休眠指令
    W25QXX_CS(GPIO_PIN_SET);
    YMX_Delay(5);
}

/**
 * @brief W25QXX唤醒
 * 
 */
void W25QXX_WakeUp(void)
{
    W25QXX_CS(GPIO_PIN_RESET);
    SPI5_ReadWriteByte(W25X_WakeUp);  //唤醒指令
    W25QXX_CS(GPIO_PIN_SET);
    YMX_Delay(5);
}
