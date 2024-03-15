#include "test_key.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"

//选择使用外部中断模式还是引脚电平检测模式，1 外部中断模式，0 电平检测模式
#define KEY_INTERRUPT 0   //由于按键2的引脚和触摸屏的INT引脚在中断线上冲突了，所以当使用触摸屏时不能使用按键2中断

void KEY_Config(void)
{
	GPIO_InitTypeDef KeyPin;
	
	KEY1_GPIO_CLK();
	KEY2_GPIO_CLK();
	
	KeyPin.Pin = KEY1_GPIO_PIN;
	KeyPin.Pull = GPIO_NOPULL;
	
	#if KEY_INTERRUPT
	  KeyPin.Mode = GPIO_MODE_IT_RISING;   //将引脚模式设置为中断模式后，会在初始化函数HAL_GPIO_Init()中映射EXTI的中断线，即配置中断源
	  KeyPin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	  
	  HAL_NVIC_SetPriority(KEY1_IRQn, 6,1);
	  HAL_NVIC_EnableIRQ(KEY1_IRQn);
	  
	  HAL_NVIC_SetPriority(KEY2_IRQn, 8,2);
	  HAL_NVIC_EnableIRQ(KEY2_IRQn);
	
	#else
	  KeyPin.Mode = GPIO_MODE_INPUT;
	#endif
	
	HAL_GPIO_Init(KEY1_GPIO_PORT, &KeyPin);   //函数内部会根据引脚模式自动配置中断源
	
	KeyPin.Pin = KEY2_GPIO_PIN;
	HAL_GPIO_Init(KEY2_GPIO_PORT, &KeyPin);
	
}

#if KEY_INTERRUPT
void KEY1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(KEY1_GPIO_PIN);
}

void KEY2_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(KEY2_GPIO_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case KEY1_GPIO_PIN: LED2_TOGGLE;
		                    break;
		case KEY2_GPIO_PIN: LED3_TOGGLE;
		                    break;
		default:LED_CYAN;
	}
}
#else
uint8_t KEY_Scan(uint16_t KEY_Pin)
{
	if(KEY_Pin == KEY1)
	{
		if(HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_PIN_SET)
		{
			while(HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_PIN_SET);
			return 1;
		}
		else
			return 0;
	}
	else if(KEY_Pin == KEY2)
	{
		if(HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == GPIO_PIN_SET)
		{
			while(HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == GPIO_PIN_SET);
			return 1;
		}
		else
			return 0;
	}
	else
	{
		printf("KEY select error.\n");
		return 0;
	}
}
#endif
