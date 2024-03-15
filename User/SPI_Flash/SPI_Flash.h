#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx.h"

#define SPIx SPI5
#define SPI_FLASH_CLK_ENABLE()   __HAL_RCC_SPI5_CLK_ENABLE()

#define SPI_FLASH_SCK_PIN GPIO_PIN_7
#define SPI_FLASH_SCK_PORT GPIOF
#define SPI_FLASH_SCK_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

#define SPI_FLASH_MOSI_PIN GPIO_PIN_8
#define SPI_FLASH_MOSI_PORT GPIOF
#define SPI_FLASH_MOSI_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

#define SPI_FLASH_MISO_PIN GPIO_PIN_9
#define SPI_FLASH_MISO_PORT GPIOF
#define SPI_FLASH_MISO_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

#define SPI_FLASH_CS_PIN GPIO_PIN_6
#define SPI_FLASH_CS_PORT GPIOF
#define SPI_FLASH_CS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

#define FLASH_JEDECID   0xEF4018

#define W25X_WriteEnable   0x06
#define W25X_WriteDisable   0x04
#define W25X_ReadStatusReg   0x05
#define W25X_WriteStatusReg   0x01
#define W25X_ReadData   0x03
#define W25X_FastReadData   0x0B
#define W25X_FastRead   0x3B
#define W25X_PageProgram   0x02
#define W25X_BlockErase   0xD8
#define W25X_SectorErase   0x20
#define W25X_ChipErase   0xC7
#define W25X_PowerDown   0xB9
#define W25X_ReleasePowerDown   0xAB
#define W25X_DeviceID   0xAB
#define W25X_ManufactDeviceID   0x90
#define W25X_JedecDeviceID   0x9F
#define W25X_Enter4ByteMode   0xB7
#define W25X_ReadStatusRegister3   0x15

#define WIP_Flag   0x01
#define Dummy_Byte   0xFF

#define SPI_FLASH_CS_ENABLE()   HAL_GPIO_WritePin(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, GPIO_PIN_RESET)
#define SPI_FLASH_CS_DISABLE()   HAL_GPIO_WritePin(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, GPIO_PIN_SET)

void SPI_Flash_Config(void);
void spi_flash_sendByte(uint8_t byteData);
uint8_t spi_flash_readByte(void);
uint8_t spi_flash_getDeviceID(void);
uint32_t spi_flash_getJedecID(void);
void spi_flash_WriteEnable(void);
void spi_flash_WriteDisable(void);
void spi_flash_WaitForWriteEnd(void);
void spi_flash_SectorErase(uint32_t EraseAddress);
void spi_flash_PageWrite(uint32_t WriteAddress, uint8_t *pData, uint16_t byteOfData);
void spi_flash_BufferWrite(uint32_t WriteAddress, uint8_t *pData, uint32_t byteOfData);
void spi_flash_BufferRead(uint32_t ReadAddress, uint8_t *pBuffer, uint32_t byteOfRead);

#endif
