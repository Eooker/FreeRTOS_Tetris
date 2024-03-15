#include "basetimer.h"

TIM_HandleTypeDef TIM_Handle;

void TIM_Config(void)
{
  TIM_CLK_ENABLE();

  TIM_Handle.Instance = TIM;

  TIM_Handle.Init.Prescaler = 9 - 1;
  TIM_Handle.Init.Period = 500 - 1;

  HAL_TIM_Base_Init(&TIM_Handle);

  HAL_NVIC_SetPriority(TIM_IRQn, 6, 2);
  HAL_NVIC_EnableIRQ(TIM_IRQn);

  HAL_TIM_Base_Start_IT(&TIM_Handle);
}


void TIM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM_Handle);
}

volatile uint32_t CPU_RunTime;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &TIM_Handle)
  {
    CPU_RunTime++;
  }
}

