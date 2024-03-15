#include "gt9xx.h"
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

extern QueueHandle_t KeyQueue_Handle;
extern uint8_t Game_over_flag;

//5寸屏GT9157驱动配置
uint8_t CTP_CFG_GT9157[] ={ 
	0x00,0x20,0x03,0xE0,0x01,0x05,0x3C,0x00,0x01,0x08,
	0x28,0x0C,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x17,0x19,0x1E,0x14,0x8B,0x2B,0x0D,
	0x33,0x35,0x0C,0x08,0x00,0x00,0x00,0x9A,0x03,0x11,
	0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
	0x00,0x20,0x58,0x94,0xC5,0x02,0x00,0x00,0x00,0x04,
	0xB0,0x23,0x00,0x93,0x2B,0x00,0x7B,0x35,0x00,0x69,
	0x41,0x00,0x5B,0x4F,0x00,0x5B,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
	0x12,0x14,0x16,0x18,0x1A,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,
	0x10,0x12,0x13,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,
	0x21,0x22,0x24,0x26,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0x48,0x01
};

//7寸屏GT911驱动配置，默认使用第一份配置文件，如果屏幕异常，可尝试使用第二份
#if  1 
uint8_t CTP_CFG_GT911[] =  {
0x41,0x20,0x03,0xE0,0x01,0x05,0x3D,0x00,0x01,0x08,
0x1E,0x05,0x3C,0x3C,0x03,0x05,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x1A,0x1C,0x1E,0x14,0x8A,0x2A,0x0C,
0x2A,0x28,0xEB,0x04,0x00,0x00,0x01,0x61,0x03,0x2C,
0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x14,0x3C,0x94,0xC5,0x02,0x08,0x00,0x00,0x04,
0xB7,0x16,0x00,0x9F,0x1B,0x00,0x8B,0x22,0x00,0x7B,
0x2B,0x00,0x70,0x36,0x00,0x70,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,
0x08,0x06,0x04,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x24,0x22,0x21,0x20,0x1F,0x1E,0x1D,0x1C,
0x18,0x16,0x13,0x12,0x10,0x0F,0x0A,0x08,0x06,0x04,
0x02,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0A,0x00
};
#else 
uint8_t CTP_CFG_GT911[] =  {
  0x00,0x20,0x03,0xE0,0x01,0x05,0x0D,0x00,0x01,0x08,
  0x28,0x0F,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8A,0x2A,0x0C,
  0x45,0x47,0x0C,0x08,0x00,0x00,0x00,0x02,0x02,0x2D,
  0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,
  0x00,0x28,0x64,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
  0x9C,0x2C,0x00,0x8F,0x34,0x00,0x84,0x3F,0x00,0x7C,
  0x4C,0x00,0x77,0x5B,0x00,0x77,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,
  0x08,0x06,0x04,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,
  0x22,0x24,0x13,0x12,0x10,0x0F,0x0A,0x08,0x06,0x04,
  0x02,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x24,0x01	
};
#endif

// 4.3寸屏GT5688驱动配置，注意GT5688第一个参数要写成0x97才会更新配置
uint8_t CTP_CFG_GT5688[] =  {
0x97,0xE0,0x01,0x10,0x01,0x05,0x0D,0x00,0x01,0x00,
0x00,0x05,0x5A,0x46,0x53,0x11,0x00,0x00,0x11,0x11,
0x14,0x14,0x14,0x22,0x0A,0x04,0x00,0x00,0x00,0x00,
0x00,0x00,0x53,0x00,0x14,0x00,0x00,0x84,0x00,0x00,
0x3C,0x00,0x00,0x64,0x1E,0x28,0x87,0x27,0x08,0x32,
0x34,0x05,0x0D,0x20,0x33,0x60,0x11,0x02,0x24,0x00,
0x00,0x64,0x80,0x80,0x14,0x02,0x00,0x00,0x54,0x89,
0x68,0x85,0x6D,0x82,0x72,0x80,0x76,0x7D,0x7B,0x7B,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x50,0x3C,
0xFF,0xFF,0x07,0x00,0x00,0x00,0x02,0x14,0x14,0x03,
0x04,0x00,0x21,0x64,0x0A,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x32,0x20,0x50,0x3C,0x3C,0x00,0x00,0x00,0x00,0x00,
0x0D,0x06,0x0C,0x05,0x0B,0x04,0x0A,0x03,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x00,0x01,0x02,0x03,0x04,0x05,
0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x3C,0x00,0x05,0x1E,0x00,0x02,
0x2A,0x1E,0x19,0x14,0x02,0x00,0x03,0x0A,0x05,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0x86,
0x22,0x03,0x00,0x00,0x33,0x00,0x0F,0x00,0x00,0x00,
0x50,0x3C,0x50,0x00,0x00,0x00,0x00,0x2A,0x01
};
//5寸屏GT917S驱动配置
uint8_t CTP_CFG_GT917S[] ={ 
  0x97,0x20,0x03,0xE0,0x01,0x0A,0x35,0x04,0x00,0x69,
  0x09,0x0F,0x50,0x32,0x33,0x11,0x00,0x32,0x11,0x11,
  0x28,0x8C,0xAA,0xDC,0x58,0x04,0x00,0x00,0x1E,0x3C,
  0x00,0x00,0x00,0x31,0x00,0x00,0x00,0x00,0x00,0x40,
  0x32,0x00,0x00,0x50,0x38,0x00,0x8D,0x20,0x16,0x4E,
  0x4C,0x7C,0x05,0x28,0x3E,0x28,0x0D,0x43,0x24,0x00,
  0x01,0x39,0x6B,0xC0,0x94,0x84,0x2D,0x00,0x54,0xB0,
  0x41,0x9D,0x49,0x8D,0x52,0x7F,0x5A,0x75,0x62,0x6C,
  0x42,0x50,0x14,0x00,0x00,0x00,0x00,0xF0,0x50,0x3C,
  0x88,0x88,0x27,0x50,0x3C,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x02,0x78,
  0x0A,0x50,0xFF,0xE4,0x04,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x3C,0xB0,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x56,0xA2,0x07,0x50,0x1E,
  0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
  0x0F,0x10,0x12,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,
  0x1D,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0x1F,0x1E,0x1D,0x1C,0x1B,0x1A,0x19,0x18,
  0x17,0x15,0x14,0x13,0x12,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0x00,0x30,0x7F,0x7F,0x7F,0xFF,
  0x54,0x64,0x00,0x80,0x46,0x07,0x50,0x3C,0x32,0x14,
  0x0A,0x64,0x32,0x00,0x00,0x00,0x00,0x11,0x02,0x62,
  0x32,0x03,0x14,0x50,0x0C,0xE2,0x14,0x50,0x00,0x54,
  0x10,0x00,0x32,0xA2,0x07,0x64,0xA4,0xB6,0x01
};

TOUCH_IC touchIC = GT9157;

const TOUCH_PARAM_TypeDef touch_param[4] = 
{
  //GT9157,5寸屏
  {
    .max_width = 800,
    .max_width = 480,
    .config_reg_addr = 0x8047
  },
  //GT911,7寸屏
  {
    .max_width = 800,
    .max_width = 480,
    .config_reg_addr = 0x8047
  },
  //GT5688,4.3寸屏
  {
    .max_width = 480,
    .max_width = 272,
    .config_reg_addr = 0x8050
  },
  //GT917S,5寸屏
  {
    .max_width = 800,
    .max_width = 480,
    .config_reg_addr = 0x8050
  },
};

uint8_t pre_touch = 0;   //上一次按下的触点数


/**
 * @brief 使用I2C进行数据传输
 * 
 * @param msgs 数据传输结构体
 * @param num 数据传输结构体的个数
 * @return int 正常完成的传输结构体个数，传输失败时返回0xff
 */
static int I2C_Transfer(struct i2c_msg *msgs, int num)
{
  int im = 0;
  int ret = 0;

  GTP_DEBUG_FUNC();

  for(im = 0; ret == 0 && im != num; im++)
  {
    if(msgs[im].flags & I2C_M_RD)   //根据msgs[im].flags判断是读数据还是写数据
    {
      ret = I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);
    }
    else
    {
      ret = I2C_WriteBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);      
    }
  }
  if(ret)   //ret不为0说明I2C传输失败，返回失败值
  {
    return ret;
  }

  return im;   //正常完成的传输结构个数
}

/**
 * @brief 从I2C设备读取数据
 * 
 * @param client_addr 设备地址
 * @param buf buf[0~1] 要读取的寄存器的起始地址   buf[2~len-1]存放读出的数据
 * @param len 寄存器地址长度 + 要读取数据的字节数
 * @return int32_t 读取成功则返回 2 ，其它值为读取失败
 */
static int32_t GTP_I2C_Read(uint8_t client_addr, uint8_t *buf, int32_t len)
{
  struct i2c_msg msgs[2];   //两个i2c_msg结构体，一个用于写入要读取的寄存器地址，一个用于读取寄存器数据
  int32_t ret = -1;
  int32_t retries = 0;   //发送不成功时重试的次数

  GTP_DEBUG_FUNC();   //输出调试信息，可忽略

  //从触控芯片读取数据时需要先写入寄存器地址
  msgs[0].flags = !I2C_M_RD;   //将控制标志设为写入数据
  msgs[0].addr = client_addr;
  msgs[0].len = GTP_ADDR_LENGTH;   //寄存器地址长度为2字节
  msgs[0].buf = &buf[0];

  //读取触控芯片寄存器数据
  msgs[1].flags = I2C_M_RD;   //将控制标志设为读取数据
  msgs[1].addr = client_addr;
  msgs[1].len = len - GTP_ADDR_LENGTH;   //要读取的数据长度
  msgs[1].buf = &buf[GTP_ADDR_LENGTH];   //在缓冲区buf内部，前面两个字节用于存放要读取的寄存器地址，后面的字节用于存放读出的数据

  while(retries < 5)
  {
    ret = I2C_Transfer(msgs, 2);   //传输i2c_msg结构体
    if(ret == 2)   //返回值为2(两个i2c_msg)说明传输成功，直接退出循环
    {
      break;
    }
    retries++;
  }
  if(retries >= 5)   //重试次数大于等于5则发送失败
  {
    GTP_ERROR("I2C Read Error");
  }
  return ret;
}

/**
 * @brief 向I2C设备写入数据
 * 
 * @param client_addr 设备地址
 * @param buf buf[0~1] 要写入的寄存器的起始地址   buf[2~len-1]要写入的数据
 * @param len 寄存器地址长度 + 要写入的数据的字节数
 * @return int32_t 写入成功则返回 1 ，其它值为写入失败
 */
static int32_t GTP_I2C_Write(uint8_t client_addr, uint8_t *buf, int32_t len)
{
  struct i2c_msg msgs;
  int32_t ret = -1;
  int32_t retries = 0;   //发送不成功时重试的次数

  GTP_DEBUG_FUNC();   //输出调试信息，可忽略

  //写入数据时也要先发送要写入的寄存器的地址，但是因为写入地址和写入数据都是写入过程，所以合并为一个传输过程
  msgs.flags = !I2C_M_RD;   //将控制标志设为写入数据
  msgs.addr = client_addr;
  msgs.len = len;   //长度直接等于 寄存器地址长度 + 要写入的字节数
  msgs.buf = buf;   //直接连续写入寄存器地址和要写入的数据

  while(retries < 5)
  {
    ret = I2C_Transfer(&msgs, 1);   //传输i2c_msg结构体
    if(ret == 1)   //返回值为1(一个i2c_msg)说明传输成功，直接退出循环
    {
      break;
    }
    retries++;
  }
  if(retries >= 5)   //重试次数大于等于5则发送失败
  {
    GTP_ERROR("I2C Read Error");
  }
  return ret;
}

struct i2c_client * i2c_connect_client = NULL; 
uint8_t config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

static int8_t GTP_I2C_Test(void);

uint8_t grp_cfg_version = 0;


int16_t pre_x[GTP_MAX_TOUCH] = {-1, -1, -1, -1, -1};
int16_t pre_y[GTP_MAX_TOUCH] = {-1, -1, -1, -1, -1};
int8_t pre_id[5] = {-1, -1, -1, -1, -1};
uint8_t queueData[5] = {0, 0, 0, 0, 5};

static void GTP_Touch_Down(int32_t id, int32_t x, int32_t y, int32_t w)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  pre_x[id] = x;
  pre_y[id] = y;

  if(!Game_over_flag)
  {
    LCD_SetTextColor(LCD_COLOR_RED);

    if(x >= 50 && x <= 150 && y >= 200 && y <= 300)   //left key
    {
      if(0 == queueData[0])
      {
        pre_id[0] = id;
        queueData[0] = 1;
        LCD_FillRect(40, 190, 120, 120, LCD_COLOR_GOLD);
        LCD_DrawRect(40, 190, 120, 120, 3);
        xQueueSendFromISR(KeyQueue_Handle, &queueData[0], &xHigherPriorityTaskWoken);
      }
    }
    else if(x >= 650 && x <= 750 && y >= 200 && y <= 300)   //right key
    {
      if(0 == queueData[1])
      {
        pre_id[1] = id;
        queueData[1] = 2;
        LCD_FillRect(640, 190, 120, 120, LCD_COLOR_GOLD);
        LCD_DrawRect(640, 190, 120, 120, 3);
        xQueueSendFromISR(KeyQueue_Handle, &queueData[1], &xHigherPriorityTaskWoken);
      }
    }
    else if(x >= 650 && x <= 750 && y >= 350 && y <= 450)   //shift key
    {
      if(0 == queueData[2])
      {
        pre_id[2] = id;
        queueData[2] = 3;
        LCD_FillRect(640, 340, 120, 120, LCD_COLOR_LIGHTBLUE);
        LCD_DrawRect(640, 340, 120, 120, 3);
        xQueueSendFromISR(KeyQueue_Handle, &queueData[2], &xHigherPriorityTaskWoken);
      }
    }
    else if(x >= 50 && x <= 150 && y >= 350 && y <= 450)   //fast down key
    {
      if(0 == queueData[3])
      {
        pre_id[3] = id;
        queueData[3] = 4;
        LCD_FillRect(40, 340, 120, 120, LCD_COLOR_VIOLET);
        LCD_DrawRect(40, 340, 120, 120, 3);
        xQueueSendFromISR(KeyQueue_Handle, &queueData[3], &xHigherPriorityTaskWoken);
      }
    }
  }
  else
  {
    if(x >= 306 && x <= 494 && y >= 255 && y <= 305)   //restart key
    {
      if(-1 == pre_id[4])
      {
        pre_id[4] = id;
				LCD_SetTextColor(LCD_COLOR_RED);
        LCD_FillRect(296, 250, 208, 60, LCD_COLOR_GOLD);
        LCD_DrawRect(296, 250, 208, 60, 3);
        LCD_SetTextColor(LCD_COLOR_BLACK);
        LCD_DisplayStringAt(340, 268, "RESTART");
      }
    }
  }

  LCD_SetTextColor(LCD_COLOR_BLACK);

  if(xHigherPriorityTaskWoken)
  {
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/*******************************************************
Function:
    Report touch release event
Input:
    ts: goodix i2c_client private data
Output:
    None.
*********************************************************/
static void GTP_Touch_Up(int32_t id)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if(!Game_over_flag)
  {
    LCD_SetTextColor(LCD_COLOR_RED);

    if(id == pre_id[0])   //left key
    {
      pre_id[0] = -1;
      LCD_FillRect(40, 190, 121, 121, LCD_COLOR_BLACK);
      LCD_FillRect(50, 200, 100, 100, LCD_COLOR_GOLD);
      LCD_DrawRect(50, 200, 100, 100, 3);
      queueData[0] = 0;
    }

    if(id == pre_id[1])   //right key
    {
      pre_id[1] = -1;
      LCD_FillRect(640, 190, 121, 121, LCD_COLOR_BLACK);
      LCD_FillRect(650, 200, 100, 100, LCD_COLOR_GOLD);
      LCD_DrawRect(650, 200, 100, 100, 3);
      queueData[1] = 0;
    }

    if(id == pre_id[2])   //shift key
    {
      pre_id[2] = -1;
      LCD_FillRect(640, 340, 121, 121, LCD_COLOR_BLACK);
      LCD_FillRect(650, 350, 100, 100, LCD_COLOR_LIGHTBLUE);
      LCD_DrawRect(650, 350, 100, 100, 3);
      queueData[2] = 0;
    }

    if(id == pre_id[3])   //fast down key
    {
      pre_id[3] = -1;
      LCD_FillRect(40, 340, 121, 121, LCD_COLOR_BLACK);
      LCD_FillRect(50, 350, 100, 100, LCD_COLOR_VIOLET);
      LCD_DrawRect(50, 350, 100, 100, 3);
      queueData[3] = 0;
    }
  }
  else
  {
    if(id == pre_id[4])   //restart key
    {
      pre_id[4] = -1;
			LCD_SetTextColor(LCD_COLOR_RED);
      LCD_FillRect(296, 250, 208, 60, LCD_COLOR_LIGHTYELLOW);
      LCD_FillRect(306, 255, 188, 50, LCD_COLOR_GOLD);
      LCD_DrawRect(306, 255, 188, 50, 3);
      LCD_SetTextColor(LCD_COLOR_BLACK);
      LCD_DisplayStringAt(340, 268, "RESTART");

      xQueueSendFromISR(KeyQueue_Handle, &queueData[4], &xHigherPriorityTaskWoken);

      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  }

  LCD_SetTextColor(LCD_COLOR_BLACK);

  pre_x[id] = -1;
  pre_y[id] = -1;
}

/*******************************************************
Function:
    Goodix touchscreen work function
Input:
    work: work struct of goodix_workqueue
Output:
    None.
*********************************************************/
static void TS_Work_Func(void)
{
  uint8_t  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};   //对buffer status位写0   每次读取完毕后需要手动将状态位清零
  uint8_t  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1]={GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
  uint8_t  touch_num = 0;
  uint8_t  finger = 0;
  // static uint16_t pre_touch = 0;
  static uint8_t pre_id[GTP_MAX_TOUCH] = {0};

    // uint8_t  key_value = 0;
  uint8_t client_addr = GTP_ADDRESS;
  uint8_t* coor_data = NULL;
  int32_t input_x = 0;
  int32_t input_y = 0;
  int32_t input_w = 0;
  int32_t id = 0;
  int32_t i  = 0;
  int32_t ret = -1;
    // struct goodix_ts_data *ts = NULL;
  
  ret = GTP_I2C_Read(client_addr, point_data, 12);
  if(ret != 2)
  {
    GTP_ERROR("I2C transfer error. errno:%d\n ", ret);
    goto exit_work_func;
  }
  
  finger = point_data[GTP_ADDR_LENGTH];   //状态寄存器数据
	
	if(finger == 0x00)   //没有数据，退出
  {
    goto exit_work_func;
  }
	
	
  if((finger & 0x80) == 0)   //buffer status位为0，说明坐标未就绪，数据无效
  {
    goto exit_work_func;
  }

  touch_num = finger & 0x0f;   //按下的坐标点数
  if (touch_num > GTP_MAX_TOUCH)   //按下的坐标点数大于最大支持的点数，错误退出
  {
    goto exit_work_func;
  }

  if (touch_num > 1)   //按下的点数不止一个
  {
    uint8_t buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};//存放所有按下的点的坐标数据(第一个点除外，已经读取了)
    ret = GTP_I2C_Read(client_addr, buf, 2 + 8 * (touch_num - 1));   //读取除第一个点外剩下点的数据
    memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1));   //将剩下的点的数据也复制到point_data里面，接在第一个点的后面
  }

  //这部分代码个人认为有点毛病，不应该根据pre_touch 和 touch_num的大小来判断是否有触点松开，因为新按下的点有可能比松开的点更多
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(pre_touch > touch_num)   //如果上一次按下的点数大于这一次按下的点数，说明有一些点松开了
  {
    for(i = 0; i < pre_touch; i++)   //遍历上一次的每个点
    {
      uint8_t j;
      for(j = 0; j < touch_num; j++)   //遍历这一次的每个点，与上一次的点进行比较
      {
        coor_data = &point_data[j * 8 + 3];   //读取该点的ID，加3表示去掉最前面的3字节(2字节地址 + 1字节状态寄存器)
        id = coor_data[0] & 0x0F;   //获取该点的ID
        if(pre_id[i] == id)   //如果上一次的和这一次的点的ID相同，说明被松开的不是这个点，跳出循环去检查下一个上次的点
        {
          break;
        }

        if(j >= touch_num - 1)   //如果遍历了当前按下的所有点都没有找到相同ID的点，说明上次按下的这个点被松开了
        {
          GTP_Touch_Up(pre_id[i]);   //执行触点松开处理程序
        }
      }
    }
  }

  if(touch_num)
  {
    for(i = 0; i < touch_num; i++)   //处理每一个按下的点
    {
      coor_data = &point_data[i * 8 + 3];   //获取指向该点的数据的指针地址，从该地址开始到后面的8个字节就是这个点的数据
      id = coor_data[0] & 0x0F;   //点的ID在第一个字节里面
      pre_id[i] = id;   //记录这一次的点的ID，以便下一次进行比较

      input_x = coor_data[1] | (coor_data[2] << 8);   //x坐标，两字节表示
      input_y = coor_data[3] | (coor_data[4] << 8);   //y坐标，两字节表示
      input_w = coor_data[5] | (coor_data[6] << 8);   //size，两字节表示

      {
        GTP_Touch_Down(id, input_x, input_y, input_w);   //执行触点按下处理程序
      }
    }
  }
  else if(pre_touch)   //这一次一个按下的点都没有，如果上一次有点按下了，说明全部松开了
  {
    for(i = 0; i < pre_touch; i++)
    {
      GTP_Touch_Up(pre_id[i]);   //执行触点松开处理程序
    }
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////

  pre_touch = touch_num;   //将这一次按下的点数记录到上一次的点数里面，以便下一次进行处理

exit_work_func:
  {
    ret = GTP_I2C_Write(client_addr, end_cmd, 3);   //将状态寄存器的buffer status位清0
    if (ret != 1)
    {
      GTP_INFO("I2C write end_cmd error!");
    }
  }

}

static int32_t GTP_Get_Info(void)
{
    uint8_t opr_buf[6] = {0};
    int32_t ret = 0;

    uint16_t abs_x_max = GTP_MAX_WIDTH;
    uint16_t abs_y_max = GTP_MAX_HEIGHT;
    uint8_t int_trigger_type = GTP_INT_TRIGGER;
        
    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+1) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+1) & 0xFF);
    
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 6);
    if (ret < 0)
    {
        return FAIL;
    }
    
    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];
    
    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+6) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+6) & 0xFF);
    
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 3);
    if (ret < 0)
    {
        return FAIL;
    }
    int_trigger_type = opr_buf[2] & 0x03;
    
    GTP_INFO("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x",
            abs_x_max,abs_y_max,int_trigger_type);
    
    return SUCCESS;    
}

/*******************************************************
Function:
    Initialize gtp.
Input:
    ts: goodix private data
Output:
    Executive outcomes.
        0: succeed, otherwise: failed
*******************************************************/
int32_t GTP_Init_Panel(void)
{
  int32_t ret = -1;

  int32_t i;
  uint8_t check_sum = 0;
  int32_t retry = 0;
  uint8_t * config;
  uint8_t * cfg_info;
  uint8_t cfg_info_len;
  uint8_t cfg_num = 0x80FE - 0x8047 + 1;   //需要配置的寄存器个数   寄存器地址0x80FF存放配置信息校验(0x8047到0x80FE之字节和的补码)

  GTP_DEBUG_FUNC();

  I2C_Touch_Init();

  ret = GTP_I2C_Test();
  if(ret < 0)
  {
    GTP_ERROR("I2C communication ERROR!");
    return ret;
  }

  GTP_Read_Version();   //获取触控芯片的型号

  if(touchIC == GT9157)
  {
    cfg_info = CTP_CFG_GT9157;   //指向寄存器配置
    cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT9157);   //计算配置表的大小
  }
  else if(touchIC == GT911)
  {
    cfg_info = CTP_CFG_GT911;
    cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT911);
  }
  else if(touchIC == GT5688)
  {
    cfg_info = CTP_CFG_GT5688;
    cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT5688);
  }

  //将配置信息复制到config里面
  memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
  memcpy(&config[GTP_ADDR_LENGTH], cfg_info, cfg_info_len);

  //计算check sum校验值，这个校验值也是要写入寄存器中的，触控芯片会对配置信息进行校验计算，只有等于我们计算的校验值才会更新配置
  if(touchIC == GT911 || touchIC == GT9157)
  {
    for(i = GTP_ADDR_LENGTH; i < cfg_num + GTP_ADDR_LENGTH; i++)
    {
      check_sum += (config[i] & 0xFF);
    }
    config[cfg_num + GTP_ADDR_LENGTH] = (~(check_sum & 0xFF)) + 1;
    config[cfg_num + GTP_ADDR_LENGTH + 1] = 1;
  }
  else if(touchIC == GT5688)
  {
    for(i = GTP_ADDR_LENGTH; i < (cfg_num + GTP_ADDR_LENGTH - 3); i += 2)
    {
      check_sum += (config[i] << 8) + config[i + 1];
    }
    check_sum = 0 - check_sum;
    GTP_DEBUG("Config checksum: 0x%04X", check_sum);

    config[(cfg_num + GTP_ADDR_LENGTH - 3)] = (check_sum >> 8) & 0xFF;
    config[(cfg_num + GTP_ADDR_LENGTH - 2)] = check_sum & 0xFF;
    config[(cfg_num + GTP_ADDR_LENGTH - 1)] = 0x01;
  }

  //将配置写入到触控芯片
  for(retry = 0; retry < 5; retry++)
  {
    ret = GTP_I2C_Write(GTP_ADDRESS, config, cfg_num + GTP_ADDR_LENGTH + 2);
    if(ret == 1)
    {
      break;
    }
  }

  HAL_Delay(10);   //等待芯片更新

  I2C_GTP_IRQEnable();   //使能中断

//  I2C_GTP_IRQDisable();

  GTP_Get_Info();

  return 0;
}

//中断处理函数
void GTP_TouchProcess(void)
{
  GTP_DEBUG_FUNC();
  TS_Work_Func();
}



/*******************************************************
Function:
    Read chip version.
Input:
    client:  i2c device
    version: buffer to keep ic firmware version
Output:
    read operation return.
        2: succeed, otherwise: failed
*******************************************************/
int32_t GTP_Read_Version(void)
{
  int32_t ret = -1;
  uint8_t buf[8] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};   //产品ID寄存器地址
  
  GTP_DEBUG_FUNC();
  
  ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
  if (ret < 0)
  {
    GTP_ERROR("GTP read version failed");
    return ret;
  }
    
  if (buf[2] == '9')
  {
    if(buf[2] == '9' && buf[3] == '1' && buf[4] == '1')
    {
      GTP_INFO("IC1 Version: %c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[7], buf[6]);
      touchIC = GT911;
      cur_lcd = INCH_7;
    }
    else if(buf[2] == '9' && buf[3] == '1' && buf[4] == '5' && buf[5] == '7')
    {
      GTP_INFO("IC2 Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
      touchIC = GT9157;
      cur_lcd = INCH_5;
    }
    else
    {
      GTP_INFO("Unknown IC Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
    }
  }
  else if(buf[2] == '5')
  {
    if(buf[2] == '5' && buf[3] == '6' && buf[4] == '8' && buf[5] == '8')
    {
      GTP_INFO("IC2 Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
      touchIC = GT5688;
      cur_lcd = INCH_4_3;
    }
    else
    {
      GTP_INFO("Unknown IC Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
    }
  }
    else
  {
    GTP_INFO("Unknown IC Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
  }
  
  return ret;
}

/*******************************************************
Function:
    I2c test Function.
Input:
    client:i2c client.
Output:
    Executive outcomes.
        2: succeed, otherwise failed.
*******************************************************/
static int8_t GTP_I2C_Test(void)
{
    uint8_t test[3] = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
    uint8_t retry = 0;
    int8_t ret = -1;
  
    GTP_DEBUG_FUNC();
  
    while(retry++ < 5)
    {
        ret = GTP_I2C_Read(GTP_ADDRESS, test, 3);
        if (ret > 0)
        {
            return ret;
        }
        GTP_ERROR("GTP i2c test failed time %d.",retry);
    }
    return ret;
}
