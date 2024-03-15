#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bsp_led.h"
#include "bsp_rcc.h"
#include "delay.h"
#include "sdram.h"
#include "bsp_lcd.h"
#include "gt9xx.h"
#include "SPI_Flash.h"
#include "bsp_debug_usart.h"
#include "test_key.h"
#include "basetimer.h"
#include "application.h"

#include "stm32f4xx.h"
#include "main.h"

//创建任务句柄
static TaskHandle_t AppTaskCreate_Handle;
static TaskHandle_t Control_Task_Handle;
static TaskHandle_t Down_Task_Handle;

QueueHandle_t KeyQueue_Handle;

//函数声明
static void AppTaskCreate(void);
static void BSP_Init(void);
static void LCD_Init(void);

int main(void)
{
	BaseType_t xReturn = pdPASS;
	
	SystemClock_Config();

	xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,
                        (const char *)"AppTaskCreate",
                        (uint32_t)128,
                        (void *)NULL,
                        (UBaseType_t)2,
												(TaskHandle_t *)&AppTaskCreate_Handle);
	
  if(pdPASS == xReturn)
		vTaskStartScheduler();
	else
		return -1;

  while(1);
}

static void BSP_Init(void)
{
  LED_GPIO_Config();

  usart_Config();
	
	KEY_Config();
	
//	TIM_Config();   //需要用的时候再初始化，不然会一直进中断，浪费系统资源
	
	SDRAM_Config();
	
	SPI_Flash_Config();   //SDRAM和Flash应该在LCD之前初始化，因为LCD需要用到这两个部分
	
	LCD_Config();
	
	GTP_Init_Panel();   //初始化触摸屏
	
	LCD_Init();

  Game_UI_Init();
}

//初始化LCD
static void LCD_Init(void)
{
	LCD_LayerInit(0, LCD_FB_START_ADDRESS, LTDC_PIXEL_FORMAT_ARGB8888);

	LCD_LayerInit(1, LCD_FB_START_ADDRESS+(LCD_GetXSize() * LCD_GetYSize() * 4), LTDC_PIXEL_FORMAT_ARGB8888);
	
	LCD_SetTransparency(1, 255);
	LCD_SetTransparency(0, 0);
	
	LCD_DisplayOn();
	
	LCD_SelectLayer(1);
	
	LCD_Clear(LCD_COLOR_WHITE);
	
	LCD_SelectLayer(0);
	
	LCD_Clear(LCD_COLOR_TRANSPARENT);
	
	LCD_SelectLayer(1);
	
	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_WHITE);
}

static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;
	
	BSP_Init();

  KeyQueue_Handle = xQueueCreate(5, 1);
	
  taskENTER_CRITICAL();
  
  xReturn = xTaskCreate((TaskFunction_t)Control_Task,
                        (const char *)"Control_Task",
                        (uint32_t)128,
                        (void *)NULL,
                        (UBaseType_t)6,
                        (TaskHandle_t)&Control_Task_Handle
  );
  if(pdPASS == xReturn)
    printf("Create Control_Task success!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t)Down_Task,
                        (const char *)"Down_Task",
                        (uint32_t)512,
                        (void *)NULL,
                        (UBaseType_t)8,
                        (TaskHandle_t)&Down_Task_Handle
  );
  if(pdPASS == xReturn)
    printf("Create Down_Task success!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle);

  taskEXIT_CRITICAL();
}
