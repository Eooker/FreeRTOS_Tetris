#ifndef __BSP_DEBUG_USART_H
#define __BSP_DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>

#define USART_Instance USART1
#define USART_CLK() __USART1_CLK_ENABLE()
#define USART_IRQn USART1_IRQn
#define USART_IRQHandler USART1_IRQHandler

#define USART_TX_GPIO_PIN GPIO_PIN_9
#define USART_TX_GPIO_PORT GPIOA
#define USART_TX_GPIO_CLK() __GPIOA_CLK_ENABLE()

#define USART_RX_GPIO_PIN GPIO_PIN_10
#define USART_RX_GPIO_PORT GPIOA
#define USART_RX_GPIO_CLK() __GPIOA_CLK_ENABLE()

void usart_Config(void);

#endif
