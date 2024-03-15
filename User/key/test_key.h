#ifndef __TEST_KEY_H
#define __TEST_KEY_H

#include "stm32f4xx.h"

#define KEY1_GPIO_PORT GPIOA
#define KEY2_GPIO_PORT GPIOC

#define KEY1_GPIO_PIN GPIO_PIN_0
#define KEY2_GPIO_PIN GPIO_PIN_13

#define KEY1_GPIO_CLK() __GPIOA_CLK_ENABLE()
#define KEY2_GPIO_CLK() __GPIOC_CLK_ENABLE()

#define KEY1_IRQn EXTI0_IRQn
#define KEY2_IRQn EXTI15_10_IRQn

#define KEY1_IRQHandler EXTI0_IRQHandler
#define KEY2_IRQHandler EXTI15_10_IRQHandler

#define KEY1 GPIO_PIN_0
#define KEY2 GPIO_PIN_13


#define KEY_ON	1
#define KEY_OFF	0

void KEY_Config(void);

#if (KEY_INTERRUPT == 0)
uint8_t KEY_Scan(uint16_t KEY_Pin);
#endif

#endif
