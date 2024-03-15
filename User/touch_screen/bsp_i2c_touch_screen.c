#include "bsp_i2c_touch_screen.h"
#include "delay.h"

I2C_HandleTypeDef I2C_Handle;


static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_Init;

  GTP_I2C_CLK_ENABLE();
  GTP_I2C_GPIO_CLK_ENABLE();

  GPIO_Init.Pin = GTP_I2C_SCL_GPIO_PIN;
  GPIO_Init.Mode = GPIO_MODE_AF_OD;
  GPIO_Init.Speed = GPIO_SPEED_HIGH;
  GPIO_Init.Pull = GPIO_NOPULL;
  GPIO_Init.Alternate = GPIO_AF4_I2C2;
  HAL_GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_Init);

  GPIO_Init.Pin = GTP_I2C_SDA_GPIO_PIN;
  HAL_GPIO_Init(GTP_I2C_SDA_GPIO_PORT, &GPIO_Init);
}

/**
 * @brief 配置与触控芯片通讯的I2C
 * 
 */
static void I2C_Mode_Config(void)
{
  I2C_Handle.Instance = GTP_I2C;
  
  I2C_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  I2C_Handle.Init.ClockSpeed = 300000;
  I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2C_Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
  I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2C_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  I2C_Handle.Init.OwnAddress1 = 0;
  I2C_Handle.Init.OwnAddress2 = 0;

  HAL_I2C_Init(&I2C_Handle);
  HAL_I2CEx_AnalogFilter_Config(&I2C_Handle, I2C_ANALOGFILTER_ENABLE);   //打开I2C模拟噪声滤波器
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  I2C_GPIO_Config();
}

/**
 * @brief 复位触摸屏控制芯片，包含上电时序，将触控芯片的I2C设备地址设为0xBA
 * 
 */
void I2C_ResetChip(void)
{
  GPIO_InitTypeDef GPIO_Init;

  GTP_RST_GPIO_CLK_ENABLE();
  GTP_INT_GPIO_CLK_ENABLE();

  GPIO_Init.Pin = GTP_RST_GPIO_PIN;
  GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Speed = GPIO_SPEED_HIGH;
  GPIO_Init.Pull = GPIO_PULLDOWN;   //RST引脚下拉，方便后面的初始化时序
  HAL_GPIO_Init(GTP_RST_GPIO_PORT, &GPIO_Init);

  GPIO_Init.Pin = GTP_INT_GPIO_PIN;
  GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Speed = GPIO_SPEED_HIGH;
  GPIO_Init.Pull = GPIO_PULLDOWN;   //INT引脚下拉，方便后面的初始化时序
  HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_Init);

  //GT9157芯片上电时序，RST保持一段时间低电平后上拉，INT一直保持低电平
  HAL_GPIO_WritePin(GTP_RST_GPIO_PORT, GTP_RST_GPIO_PIN, GPIO_PIN_RESET);
  delay_us(500);

  HAL_GPIO_WritePin(GTP_RST_GPIO_PORT, GTP_RST_GPIO_PIN, GPIO_PIN_SET);
  HAL_Delay(10);

  //将INT引脚配置成浮空输入模式，以便接收触摸中断信号
  GPIO_Init.Pin = GTP_INT_GPIO_PIN;
  GPIO_Init.Mode = GPIO_MODE_INPUT;
  GPIO_Init.Speed = GPIO_SPEED_HIGH;
  GPIO_Init.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_Init);
}

/**
 * @brief 打开触摸屏中断
 * 
 */
void I2C_GTP_IRQEnable(void)
{
  GPIO_InitTypeDef GPIO_Init;

  GTP_INT_GPIO_CLK_ENABLE();

  GPIO_Init.Pin = GTP_INT_GPIO_PIN;
  GPIO_Init.Mode = GPIO_MODE_IT_RISING;
  GPIO_Init.Speed = GPIO_SPEED_HIGH;
  GPIO_Init.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_Init);

  HAL_NVIC_SetPriority(GTP_INT_EXTI_IRQ, 5, 1);
  HAL_NVIC_EnableIRQ(GTP_INT_EXTI_IRQ);
}

/**
 * @brief 关闭触摸屏中断
 * 
 */
void I2C_GTP_IRQDisable(void)
{
  GPIO_InitTypeDef GPIO_Init;

  GTP_INT_GPIO_CLK_ENABLE();

  GPIO_Init.Pin = GTP_INT_GPIO_PIN;
  GPIO_Init.Mode = GPIO_MODE_INPUT;
  GPIO_Init.Speed = GPIO_SPEED_HIGH;
  GPIO_Init.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_Init);

  HAL_NVIC_SetPriority(GTP_INT_EXTI_IRQ, 1, 1);
  HAL_NVIC_DisableIRQ(GTP_INT_EXTI_IRQ);
}

/**
 * @brief 初始化触摸屏
 * 
 */
void I2C_Touch_Init(void)
{
  I2C_Mode_Config();   //配置I2C通讯
  I2C_ResetChip();   //复位触控芯片并设置它的I2C设备地址
  I2C_GTP_IRQEnable();   //使能触摸中断
//  I2C_GTP_IRQDisable();
}

/**
 * @brief 使用I2C读取数据
 * 
 * @param ClientAddr 从设备地址
 * @param pBuffer 数据缓冲区指针
 * @param NumByteToRead 需要读取的字节数
 * @return uint32_t 读取成功返回0，否则返回0xff
 */
uint32_t I2C_ReadBytes(uint8_t ClientAddr, uint8_t * pBuffer, uint16_t NumByteToRead)
{
  uint8_t ret = HAL_I2C_Master_Receive(&I2C_Handle, ClientAddr, pBuffer, NumByteToRead, 1000);
  if(ret == HAL_OK)
  {
    return 0;
  }
  else
  {
    return 0xff;
  }
}

/**
 * @brief 使用I2C写入数据
 * 
 * @param ClientAddr 从设备地址
 * @param pBuffer 数据缓冲区指针
 * @param NumByteToRead 需要写入的字节数
 * @return uint32_t 写入成功返回0，否则返回0xff
 */
uint32_t I2C_WriteBytes(uint8_t ClientAddr, uint8_t * pBuffer, uint16_t NumByteToRead)
{
  uint8_t ret = HAL_I2C_Master_Transmit(&I2C_Handle, ClientAddr, pBuffer, NumByteToRead, 1000);
  if(ret == HAL_OK)
  {
    return 0;
  }
  else
  {
    return 0xff;
  }
}

/**
 * @brief 触摸中断服务函数
 * 
 */
void GTP_IRQHandler(void)
{
  if(__HAL_GPIO_EXTI_GET_IT(GTP_INT_GPIO_PIN) != RESET)
  {
    GTP_TouchProcess();
    __HAL_GPIO_EXTI_CLEAR_IT(GTP_INT_GPIO_PIN);
  }
}




