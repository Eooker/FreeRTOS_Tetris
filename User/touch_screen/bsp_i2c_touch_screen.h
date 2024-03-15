#ifndef __BSP_I2C_TOUCH_SCREEN_H
#define __BSP_I2C_TOUCH_SCREEN_H

#include "stm32f4xx.h"
#include "gt9xx.h"
#include "bsp_led.h"

#define GTP_ADDRESS 0xBA

#define GTP_I2C I2C2
#define GTP_I2C_CLK_ENABLE()   __HAL_RCC_I2C2_CLK_ENABLE()

#define GTP_I2C_SCL_GPIO_PIN   GPIO_PIN_4
#define GTP_I2C_SCL_GPIO_PORT   GPIOH

#define GTP_I2C_SDA_GPIO_PIN   GPIO_PIN_5
#define GTP_I2C_SDA_GPIO_PORT   GPIOH

#define GTP_I2C_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()

#define GTP_RST_GPIO_PIN   GPIO_PIN_11
#define GTP_RST_GPIO_PORT   GPIOD
#define GTP_RST_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()

#define GTP_INT_GPIO_PIN   GPIO_PIN_13
#define GTP_INT_GPIO_PORT   GPIOD
#define GTP_INT_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()
#define GTP_INT_EXTI_IRQ   EXTI15_10_IRQn

#define GTP_IRQHandler   EXTI15_10_IRQHandler


void I2C_ResetChip(void);
void I2C_GTP_IRQEnable(void);
void I2C_GTP_IRQDisable(void);
void I2C_Touch_Init(void);
uint32_t I2C_ReadBytes(uint8_t ClientAddr, uint8_t * pBuffer, uint16_t NumByteToRead);
uint32_t I2C_WriteBytes(uint8_t ClientAddr, uint8_t * pBuffer, uint16_t NumByteToRead);

#endif
