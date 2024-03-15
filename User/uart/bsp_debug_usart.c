#include "bsp_debug_usart.h"
#include "bsp_led.h"

#if 1
#if (__ARMCC_VERSION >= 6010050)                    /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");          /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");            /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义. */
FILE __stdout;

#endif



UART_HandleTypeDef UartHandle;

uint8_t rxbuff[1];
uint8_t rxdata[100];
uint8_t onebuff[] = "This is I had recevied: ";

void usart_Config(void)
{
  UartHandle.Instance = USART_Instance;

  UartHandle.Init.BaudRate = 115200;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Mode = UART_MODE_TX_RX;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_8;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;

  HAL_UART_Init(&UartHandle);

  // __HAL_USART_ENABLE_IT(&UartHandle, USART_IT_RXNE);

  // HAL_UART_Receive_IT(&UartHandle, rxbuff, 1);
}

void HAL_UART_MspInit(UART_HandleTypeDef *husart)
{
  GPIO_InitTypeDef usartPin;

  USART_TX_GPIO_CLK();
  USART_RX_GPIO_CLK();
  USART_CLK();

  usartPin.Pin = USART_TX_GPIO_PIN;
  usartPin.Mode = GPIO_MODE_AF_PP;
  usartPin.Pull = GPIO_NOPULL;
  usartPin.Speed = GPIO_SPEED_FREQ_HIGH;
  usartPin.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(USART_TX_GPIO_PORT, &usartPin);

  usartPin.Pin = USART_RX_GPIO_PIN;
  HAL_GPIO_Init(USART_RX_GPIO_PORT, &usartPin);

  HAL_NVIC_SetPriority(USART_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(USART_IRQn);
}

void USART_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UartHandle);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *husart)
{
  static uint8_t rx_count = 0;
  rxdata[rx_count] = rxbuff[0];
  if(rxdata[rx_count] == '\n')
  {
    HAL_UART_Transmit(&UartHandle, onebuff, sizeof("This is I had recevied: "), 1000);
    HAL_UART_Transmit(&UartHandle, rxdata, rx_count+1, 1000);
		
		for(;rx_count>0;rx_count--)
		{
			rxdata[rx_count] = '\0';
		}
		rxdata[0] = '\0';
  }
	else
	{
		rx_count++;
	}
	HAL_UART_Receive_IT(&UartHandle, rxbuff, 1);
}

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);
	return ch;
}

//关闭半主机模式后只能配置printf函数的重定向，不能配置scanf的重定向，因为单片
//机上没有scanf的输入设备，只有PC上才有，所以要使用scanf只能自己定义这个函数
//int fgetc(FILE *f)
//{
//	int ch;
//  while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) == RESET);
//	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);
//	return ch;
//}
