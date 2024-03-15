#include "SPI_Flash.h"
#include <stdio.h>

SPI_HandleTypeDef SPI_Handle;

void SPI_Flash_Config(void)
{
  SPI_FLASH_CLK_ENABLE();

  SPI_Handle.Instance = SPIx;

  SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;   //数据传输时MSB先行
  SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;   //数据长度选择8位数据
  SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;   //波特率2分频
  SPI_Handle.Init.Direction = SPI_DIRECTION_2LINES;   //使用双线(MOSI MISO)全双工模式
  SPI_Handle.Init.Mode = SPI_MODE_MASTER;   //MCU作为主机
  SPI_Handle.Init.NSS = SPI_NSS_SOFT;   //片选信号CS使用软件模式，即用户自己控制该信号
  SPI_Handle.Init.TIMode = SPI_TIMODE_DISABLE;   //不使用TI模式
  SPI_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   //不使用CRC校验
  SPI_Handle.Init.CRCPolynomial = 5;   //由于不使用CRC校验，所以这个数字随便填不影响
  SPI_Handle.Init.CLKPhase = SPI_PHASE_2EDGE;   //偶数边沿采样
  SPI_Handle.Init.CLKPolarity = SPI_POLARITY_HIGH;   //CLK空闲时为高电平

  HAL_SPI_Init(&SPI_Handle);

  __HAL_SPI_ENABLE(&SPI_Handle);   //将SPI控制寄存器中的使能位置1，使能SPI
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef SPI_GPIO_Handle;

  SPI_FLASH_SCK_GPIO_CLK_ENABLE();
  SPI_FLASH_MOSI_GPIO_CLK_ENABLE();
  SPI_FLASH_MISO_GPIO_CLK_ENABLE();
  SPI_FLASH_CS_GPIO_CLK_ENABLE();

  SPI_GPIO_Handle.Pin = SPI_FLASH_MOSI_PIN;
  SPI_GPIO_Handle.Mode = GPIO_MODE_AF_PP;
  SPI_GPIO_Handle.Speed = GPIO_SPEED_HIGH;
  SPI_GPIO_Handle.Pull = GPIO_PULLUP;
  SPI_GPIO_Handle.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(SPI_FLASH_MOSI_PORT, &SPI_GPIO_Handle);

  SPI_GPIO_Handle.Pin = SPI_FLASH_MISO_PIN;
  HAL_GPIO_Init(SPI_FLASH_MISO_PORT, &SPI_GPIO_Handle);

  SPI_GPIO_Handle.Pin = SPI_FLASH_SCK_PIN;
  HAL_GPIO_Init(SPI_FLASH_SCK_PORT, &SPI_GPIO_Handle);

  SPI_GPIO_Handle.Pin = SPI_FLASH_CS_PIN;
  SPI_GPIO_Handle.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(SPI_FLASH_CS_PORT, &SPI_GPIO_Handle);
}

/**
 * @brief 向flash发送一个字节的数据
 * 
 * @param byteData 字节数据
 */
void spi_flash_sendByte(uint8_t byteData)
{
  HAL_StatusTypeDef statusReturn;

  statusReturn = HAL_SPI_Transmit(&SPI_Handle, &byteData, 1, 1000);

  if(statusReturn != HAL_OK)
  {
    printf("SPI传输失败 原因为: %d\n", statusReturn);
  }
}

/**
 * @brief 从flash读取一个字节数据
 * 
 * @return uint8_t 返回读取到的字节数据
 */
uint8_t spi_flash_readByte(void)
{
  uint8_t readByte = 0;
  HAL_StatusTypeDef statusReturn;

  statusReturn = HAL_SPI_Receive(&SPI_Handle, &readByte, 1, 1000);

  if(statusReturn != HAL_OK)
  {
    printf("SPI传输失败 原因为: %d", statusReturn);
    return 0xff;
  }

  return readByte;
}

/**
 * @brief 获取Flash设备ID
 * 
 * @return uint8_t 返回设备ID号
 */
uint8_t spi_flash_getDeviceID(void)
{
  uint8_t DeviceID;

  SPI_FLASH_CS_ENABLE();   //拉低片选信号，开始SPI传输

  spi_flash_sendByte(W25X_DeviceID);

  spi_flash_sendByte(Dummy_Byte);
  spi_flash_sendByte(Dummy_Byte);
  spi_flash_sendByte(Dummy_Byte);

  DeviceID = spi_flash_readByte();

  SPI_FLASH_CS_DISABLE();   //拉高片选信号，结束SPI传输

  return DeviceID;
}

/**
 * @brief 获取Flash设备的JedecID
 * 
 * @return uint32_t 返回设备的JedecID号
 */
uint32_t spi_flash_getJedecID(void)
{
	uint32_t JedecID, dataHigh, dataMiddle, dataLow;

  SPI_FLASH_CS_ENABLE();   //拉低片选信号，开始SPI传输

  spi_flash_sendByte(W25X_JedecDeviceID);
	
	dataHigh = spi_flash_readByte();
	
	dataMiddle = spi_flash_readByte();
	
	dataLow = spi_flash_readByte();
	
	SPI_FLASH_CS_DISABLE();   //拉高片选信号，结束SPI传输

  JedecID = (dataHigh << 16) | (dataMiddle << 8) | dataLow;

  return JedecID;
}

/**
 * @brief flash写使能
 * 
 */
void spi_flash_WriteEnable(void)
{
  SPI_FLASH_CS_ENABLE();

  spi_flash_sendByte(W25X_WriteEnable);

  SPI_FLASH_CS_DISABLE();
}

/**
 * @brief flash写失能
 * 
 */
void spi_flash_WriteDisable(void)
{
  SPI_FLASH_CS_ENABLE();

  spi_flash_sendByte(W25X_WriteDisable);

  SPI_FLASH_CS_DISABLE();
}

/**
 * @brief 等待flash完成写入
 * 
 */
void spi_flash_WaitForWriteEnd(void)
{
  uint8_t writeStatus = 0;
  SPI_FLASH_CS_ENABLE();

  spi_flash_sendByte(W25X_ReadStatusReg);

  do
  {
    writeStatus = spi_flash_readByte();
  }while((writeStatus & WIP_Flag) == SET);

  SPI_FLASH_CS_DISABLE();
}

/**
 * @brief 擦除flash的一个扇区
 * 
 * @param EraseAddress 需要擦除的flash内部地址
 */
void spi_flash_SectorErase(uint32_t EraseAddress)
{
  spi_flash_WriteEnable();
  spi_flash_WaitForWriteEnd();
  SPI_FLASH_CS_ENABLE();

  spi_flash_sendByte(W25X_SectorErase);
  // spi_flash_sendByte((EraseAddress >> 24));   //W25Q256内部地址有32位，需要分开传输
  spi_flash_sendByte(((EraseAddress & 0x00ff0000) >> 16));
  spi_flash_sendByte(((EraseAddress & 0x0000ff00) >> 8));
  spi_flash_sendByte( (EraseAddress & 0x000000ff) );

  SPI_FLASH_CS_DISABLE();

  spi_flash_WaitForWriteEnd();
}

/**
 * @brief 向flash的指定地址写入不超过256字节的数据
 * 
 * @param WriteAddress 需要写入数据的flash内部地址
 * @param pData 需要写入的数据的指针
 * @param byteOfData 数据的字节数
 */
void spi_flash_PageWrite(uint32_t WriteAddress, uint8_t *pData, uint16_t byteOfData)
{
  if(byteOfData > 256)
  {
    printf("写入的字节数据太多 最多只能是256个 \n");
    return;
  }

  spi_flash_WriteEnable();

  SPI_FLASH_CS_ENABLE();

  spi_flash_sendByte(W25X_PageProgram);
  // spi_flash_sendByte((WriteAddress >> 24));   //W25Q256内部地址有32位，需要分开传输
  spi_flash_sendByte(((WriteAddress & 0x00ff0000) >> 16));
  spi_flash_sendByte(((WriteAddress & 0x0000ff00) >> 8));
  spi_flash_sendByte( (WriteAddress & 0x000000ff) );

  while(byteOfData > 0)   //开始传输数据
  {
    spi_flash_sendByte(*pData);
    byteOfData--;
    pData++;
  }

  SPI_FLASH_CS_DISABLE();

  spi_flash_WaitForWriteEnd();   //等待数据传输完成
}

/**
 * @brief 向flash的指定地址写入任意长度的数据
 * 
 * @param WriteAddress 需要写入数据的flash内部地址
 * @param pData 需要写入的数据的指针
 * @param byteOfData 数据的字节数
 */
void spi_flash_BufferWrite(uint32_t WriteAddress, uint8_t *pData, uint32_t byteOfData)
{
  uint8_t Addmod = WriteAddress % 256;
  uint8_t remainSize = 256 - Addmod;   //当前页剩余的字节数
  uint32_t NumOfPage, NumOfSingle;
  NumOfPage = byteOfData / 256;
  NumOfSingle = byteOfData % 256;

  if(Addmod != 0)   //如果写入地址不对齐，则先进行对齐处理
  {
    if(byteOfData <= remainSize)   //当前页的剩余内存足够将数据全部写完，则直接写入，然后退出
    {
      spi_flash_PageWrite(WriteAddress, pData, byteOfData);
      return;
    }
    else   //先将当前页剩余内存全部写完，然后重新计数NumOfPage和NumOfSingle
    {
      spi_flash_PageWrite(WriteAddress, pData, remainSize);
      WriteAddress += remainSize;
      pData += remainSize;

      NumOfPage = (byteOfData - remainSize) / 256;
      NumOfSingle = (byteOfData - remainSize) % 256;   //现在写入地址是对齐的
    }
  }

  if(NumOfPage != 0)   //如果有整页的数据，先把整页数据写完
  {
    while(NumOfPage--)
    {
      spi_flash_PageWrite(WriteAddress, pData, 256);
      WriteAddress += 256;
      pData += 256;
    }
  }

  if(NumOfSingle != 0)   //把不足一页的数据写完
  {
    spi_flash_PageWrite(WriteAddress, pData, NumOfSingle);
  }

}

/**
 * @brief 从flash指定地址处读取任意长度的数据
 * 
 * @param ReadAddress 读取地址
 * @param pBuffer 缓存数据的指针
 * @param byteOfRead 读取的字节数
 */
void spi_flash_BufferRead(uint32_t ReadAddress, uint8_t *pBuffer, uint32_t byteOfRead)
{
  SPI_FLASH_CS_ENABLE();

  spi_flash_sendByte(W25X_ReadData);
  // spi_flash_sendByte((ReadAddress >> 24));   //W25Q256内部地址有32位，需要分开传输
  spi_flash_sendByte(((ReadAddress & 0x00ff0000) >> 16));
  spi_flash_sendByte(((ReadAddress & 0x0000ff00) >> 8));
  spi_flash_sendByte( (ReadAddress & 0x000000ff) );

  while(byteOfRead--)
  {
    *pBuffer = spi_flash_readByte();   //读取数据
    pBuffer++;
  }

  SPI_FLASH_CS_DISABLE();
}
