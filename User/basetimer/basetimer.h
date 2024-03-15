#ifndef __BASETIMER_H
#define __BASETIMER_H

#include "stm32f4xx.h"

#define TIM TIM6
#define TIM_IRQn TIM6_DAC_IRQn
#define TIM_IRQHandler TIM6_DAC_IRQHandler
#define TIM_CLK_ENABLE() __TIM6_CLK_ENABLE()

void TIM_Config(void);

#endif
