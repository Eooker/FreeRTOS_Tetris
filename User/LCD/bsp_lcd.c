#include "bsp_lcd.h"
#include "sdram.h"
#include "string.h"
#include "SPI_Flash.h"

#include "font24.c"
#include "font20.c"
#include "font16.c"
#include "font12.c"
#include "font8.c"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

static LTDC_HandleTypeDef LTDC_Handle;
static DMA2D_HandleTypeDef DMA2D_Handle;

static uint32_t ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

static void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c);
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
static void LTDC_GPIO_Config(void);
static void LCD_CLK_Config(void);
static void LTDC_Config(void);
static int GetGB2312Code_from_EXFlash(uint8_t * pBuffer, uint16_t c);

LCD_TypeDef cur_lcd = INCH_5;

void LCD_Config(void)
{
  LTDC_GPIO_Config();

  LCD_CLK_Config();

  LTDC_Config();

  __DMA2D_CLK_ENABLE();
}

static void LTDC_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  LTDC_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;   //除R3、R6、G3引脚外都使用AF14_LTDC

  //红色数据线
  GPIO_InitStruct.Pin = LTDC_R0_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_R1_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_R2_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_R4_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_R5_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_R7_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);

  //绿色数据线
  GPIO_InitStruct.Pin = LTDC_G0_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_G1_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_G2_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_G4_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_G5_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_G6_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_G7_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);

  //蓝色数据线
  GPIO_InitStruct.Pin = LTDC_B0_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_B1_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_B2_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_B3_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_B4_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_B5_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_B6_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_B7_GPIO_Pin;
  HAL_GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);

  //控制信号线
  GPIO_InitStruct.Pin = LTDC_CLK_GPIO_Pin;
  HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_HSYNC_GPIO_Pin;
  HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_VSYNC_GPIO_Pin;
  HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_DE_GPIO_Pin;
  HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);

  //下面的这3个引脚使用AF9_LTDC
  GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;

  GPIO_InitStruct.Pin = LTDC_R3_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_R6_GPIO_Pin;
  HAL_GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_G3_GPIO_Pin;
  HAL_GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);

  //液晶使能信号和背光使能信号使用普通的推挽输出模式
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = LTDC_DISP_GPIO_Pin;
  HAL_GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LTDC_BL_GPIO_Pin;
  HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief STM32与液晶屏通讯的时钟配置
 * 时钟配置为9.6MHz
 */
static void LCD_CLK_Config(void)
{
  RCC_PeriphCLKInitTypeDef LCD_CLK_Init;

  LCD_CLK_Init.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  LCD_CLK_Init.PLLSAI.PLLSAIN = 192;
  LCD_CLK_Init.PLLSAI.PLLSAIR = 5;
  LCD_CLK_Init.PLLSAIDivR = RCC_PLLSAIDIVR_4;

  HAL_RCCEx_PeriphCLKConfig(&LCD_CLK_Init);
}

static void LTDC_Config(void)
{
  __LTDC_CLK_ENABLE();

  LTDC_Handle.Instance = LTDC;

  //配置通讯时序参数
  LTDC_Handle.Init.HorizontalSync = HSW - 1;   //行同步信号宽度
  LTDC_Handle.Init.VerticalSync = VSW - 1;   //垂直同步信号宽度
  LTDC_Handle.Init.AccumulatedHBP = HSW + HBP - 1;
  LTDC_Handle.Init.AccumulatedVBP = VSW + VBP - 1;
  LTDC_Handle.Init.AccumulatedActiveW = HSW + HBP + LCD_PIXEL_WIDTH - 1;
  LTDC_Handle.Init.AccumulatedActiveH = VSW + VBP + LCD_PIXEL_HEIGHT - 1;
  LTDC_Handle.Init.TotalWidth = HSW + HBP + LCD_PIXEL_WIDTH + HFP - 1;
  LTDC_Handle.Init.TotalHeigh = VSW + VBP + LCD_PIXEL_HEIGHT + VFP - 1;

  //配置LCD背景颜色，默认黑色
  LTDC_Handle.Init.Backcolor.Red = 0;
  LTDC_Handle.Init.Backcolor.Green = 0;
  LTDC_Handle.Init.Backcolor.Blue = 0;

  //配置信号极性
  LTDC_Handle.Init.HSPolarity = LTDC_HSPOLARITY_AL;   //配置行同步极性，低电平有效
  LTDC_Handle.Init.VSPolarity = LTDC_VSPOLARITY_AL;   //配置垂直同步极性，低电平有效
  LTDC_Handle.Init.DEPolarity = LTDC_DEPOLARITY_AL;   //配置数据有效极性，低电平有效
  LTDC_Handle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;   //配置像素时钟极性，同输入时钟

  //配置LCD的像素宽度和高度
  LTDC_Handle.LayerCfg->ImageWidth = LCD_PIXEL_WIDTH;
  LTDC_Handle.LayerCfg->ImageHeight = LCD_PIXEL_HEIGHT;

  HAL_LTDC_Init(&LTDC_Handle);
	
	LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
 * @brief 获取LCD当前层X轴的大小
 * 
 * @return uint32_t X轴的大小
 */
uint32_t LCD_GetXSize(void)
{
  return LTDC_Handle.LayerCfg[ActiveLayer].ImageWidth;
}

/**
 * @brief 获取LCD当前层Y轴的大小
 * 
 * @return uint32_t Y轴的大小
 */
uint32_t LCD_GetYSize(void)
{
  return LTDC_Handle.LayerCfg[ActiveLayer].ImageHeight;
}

/**
 * @brief 设置LCD当前层X轴的大小
 * 
 * @param imageHeightPixels 图像宽度像素个数
 */
void LCD_SetXSize(uint32_t imageWidthPixels)
{
  LTDC_Handle.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}

/**
 * @brief 设置LCD当前层Y轴的大小
 * 
 * @param imageHeightPixels 图像高度像素个数
 */
void LCD_SetYSize(uint32_t imageHeightPixels)
{
  LTDC_Handle.LayerCfg[ActiveLayer].ImageWidth = imageHeightPixels;
}


/**
 * @brief 层级初始化
 * 
 * @param LayerIndex 要设置的层
 * @param FB_Address 该层的显存首地址
 * @param PixelFormat 该层的像素格式
 */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address, uint32_t PixelFormat)
{
  LTDC_LayerCfgTypeDef Layer_cfg;

  //设置窗口起始位置和结束位置
  Layer_cfg.WindowX0 = 0;    //窗口起始位置X坐标
  Layer_cfg.WindowX1 = LCD_GetXSize();   //窗口结束位置X坐标
  Layer_cfg.WindowY0 = 0;   //窗口起始位置Y坐标
  Layer_cfg.WindowY1 = LCD_GetYSize();   //窗口结束位置Y坐标

  //设置像素格式和显存首地址
  Layer_cfg.PixelFormat = PixelFormat;   //像素格式
  Layer_cfg.FBStartAdress = FB_Address;   //这一层的显存首地址

  //设置这一层的背景颜色
  Layer_cfg.Backcolor.Blue = 0;
  Layer_cfg.Backcolor.Green = 0;
  Layer_cfg.Backcolor.Red = 0;

  //设置混合运算参数
  Layer_cfg.Alpha = 255;   //用于混合的透明度常量，范围(0-255)，0为完全透明
  Layer_cfg.Alpha0 = 0;   //默认透明度常量，不参与混合运算，在层窗口外或层禁止时使用，范围(0-255)，0为完全透明
  Layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;   //这一层的混合系数1
  Layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;   //这一层的混合系数2

  //设置图像的宽度和高度
  Layer_cfg.ImageWidth = LCD_GetXSize();
  Layer_cfg.ImageHeight = LCD_GetYSize();

  HAL_LTDC_ConfigLayer(&LTDC_Handle, &Layer_cfg, LayerIndex);   //配置这一层的参数

  //设置这一层的字体和字体背景
  DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;   //字体颜色
  DrawProp[LayerIndex].pFont = &LCD_DEFAULT_FONT;   //字体类型
  DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK;   //字体背景颜色

  __HAL_LTDC_RELOAD_CONFIG(&LTDC_Handle);   //重载这一层的配置参数
}

/**
 * @brief 选择LCD的当前层
 * 
 * @param LayerIndex 1：前景层   0：背景层
 */
void LCD_SelectLayer(uint32_t LayerIndex)
{
  ActiveLayer = LayerIndex;
}

/**
 * @brief 设置LCD层的可视化(即禁止或使能某一层)
 * 
 * @param LayerIndex 1：前景层   0：背景层
 * @param State 禁止或使能
 */
void LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State)
{
  if(State == ENABLE)
  {
    __HAL_LTDC_LAYER_ENABLE(&LTDC_Handle, LayerIndex);
  }
  else
  {
    __HAL_LTDC_LAYER_DISABLE(&LTDC_Handle, LayerIndex);
  }

  __HAL_LTDC_RELOAD_CONFIG(&LTDC_Handle);
}

/**
 * @brief 设置LCD的透明度常量，即Alpha
 * 
 * @param LayerIndex 1：前景层   0：背景层
 * @param Transparency 透明度，范围(0-255)，0为完全透明
 */
void LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{
  HAL_LTDC_SetAlpha(&LTDC_Handle, Transparency, LayerIndex);
}

/**
 * @brief 设置LCD缓冲帧的首地址
 * 
 * @param LayerIndex 1：前景层   0：背景层
 * @param Address LCD缓冲帧的首地址
 */
void LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address)
{
  HAL_LTDC_SetAddress(&LTDC_Handle, Address, LayerIndex);
}

/**
 * @brief 设置显示窗口
 * 
 * @param LayerIndex 
 * @param Xpos 窗口起始位置X坐标
 * @param Ypos 窗口起始位置Y坐标
 * @param Width 窗口宽度
 * @param Height 窗口高度
 */
void LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  HAL_LTDC_SetWindowSize(&LTDC_Handle, Width, Height, LayerIndex);   //重新设置窗口大小

  HAL_LTDC_SetWindowPosition(&LTDC_Handle, Xpos, Ypos, LayerIndex);   //重新设置窗口的起始位置
}

/**
 * @brief 设置LCD当前层的文字颜色
 * 
 * @param Color 文字颜色
 */
void LCD_SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
 * @brief 获取LCD当前层的文字颜色
 * 
 * @return uint32_t 文字颜色
 */
uint32_t LCD_GetTextColor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
 * @brief 设置LCD当前层的文字背景颜色
 * 
 * @param Color 文字背景颜色
 */
void LCD_SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
 * @brief 获取LCD当前层的文字背景颜色
 * 
 * @return uint32_t 文字背景颜色
 */
uint32_t LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
 * @brief 设置LCD当前层的文字颜色和文字背景颜色
 * 
 * @param TextColor 文字颜色
 * @param BackColor 文字背景颜色
 */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor)
{
	LCD_SetTextColor(TextColor);
	LCD_SetBackColor(BackColor);
}

/**
 * @brief 设置LCD当前层显示的字体
 * 
 * @param fonts 字体类型
 */
void LCD_SetFont(sFONT *fonts)
{
	DrawProp[ActiveLayer].pFont = fonts;
}

/**
 * @brief 获取LCD当前层显示的字体
 * 
 * @return sFONT* LCD当前层的字体类型
 */
sFONT *LCD_GetFont(void)
{
  return DrawProp[ActiveLayer].pFont;
}

/**
 * @brief 读取LCD的像素值
 * 
 * @param Xpos 需要读取的像素点的X坐标
 * @param Ypos 需要读取的像素点的Y坐标
 * @return uint32_t 指定像素点的像素值
 */
uint32_t LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint32_t retval = 0;

  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:  //ARGB8888格式占用4个字节，要读取的像素点地址等于(显存首地址 + 偏移像素个数 * 4字节)，其他格式同理
           retval = *(__IO uint32_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (4 * (Ypos * LCD_GetXSize() + Xpos)));
           break;
    case LTDC_PIXEL_FORMAT_RGB888:
           retval = (*(__IO uint8_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (3 * (Ypos * LCD_GetXSize() + Xpos)) + 2)) << 16;
           retval |= (*(__IO uint8_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (3 * (Ypos * LCD_GetXSize() + Xpos)) + 1)) << 8;
           retval |= *(__IO uint8_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (3 * (Ypos * LCD_GetXSize() + Xpos)) + 0);
           break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:
           retval = *(__IO uint16_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos)));
           break;
    default:
           retval = *(__IO uint16_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos)));
           break;
  }

  return retval;
}

/**
 * @brief LCD当前层清屏
 * 
 * @param Color 用于填充的背景颜色
 */
void LCD_Clear(uint32_t Color)
{
	LL_FillBuffer(ActiveLayer, (uint32_t *)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress), LCD_GetXSize(), LCD_GetYSize(), 0, Color);
}

/**
 * @brief 清除一行
 * 
 * @param Line 要清除的行
 */
void LCD_ClearLine(uint32_t Line)
{
  uint32_t colorBackup = DrawProp[ActiveLayer].TextColor;   //备份原来的字体颜色

  DrawProp[ActiveLayer].TextColor = DrawProp[ActiveLayer].BackColor;   //用实心矩形覆盖掉要清除的行，所以颜色必须是背景色

  //用一行跟背景色一致的实心矩形来填充要清除的行
  LCD_FillRect(0, (Line * DrawProp[ActiveLayer].pFont->Height), LCD_GetXSize(), DrawProp[ActiveLayer].pFont->Height, DrawProp[ActiveLayer].TextColor);   //单行的高度为字体高度

  //恢复字体颜色
  DrawProp[ActiveLayer].TextColor = colorBackup;
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
}

/**
 * @brief 填充一个实心矩形
 * 
 * @param Xpos 矩形起始位置的X坐标
 * @param Ypos 矩形起始位置的Y坐标
 * @param Width 矩形宽度
 * @param Height 矩形高度
 * @param Color 填充颜色
 */
void LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint32_t Color)
{
  uint32_t StartAddress = 0;
  uint8_t temp = 1;

  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);

  //获取矩形起始位置的地址
  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:temp = 4;break;
    case LTDC_PIXEL_FORMAT_RGB888:temp = 3;break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:temp = 2;break;
    default:temp = 2;break;
  }
  StartAddress = LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (temp * (Ypos * LCD_GetXSize() + Xpos));

  LL_FillBuffer(ActiveLayer, (uint32_t *)StartAddress, Width, Height, (LCD_GetXSize() - Width), Color);   //填充矩形
}

/**
 * @brief 绘制一个矩形
 * 
 * @param Xpos 矩形起点x坐标
 * @param Ypos 矩形起点y坐标
 * @param Width 矩形宽度(x轴长度)
 * @param Height 矩形高度(y轴长度)
 * @param Line_width 线条粗细
 */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint8_t Line_width)
{
  while(Line_width--)
  {
    LCD_DrawHLine(Xpos + Line_width, Ypos + Line_width, Width - Line_width);
    LCD_DrawVLine(Xpos + Line_width, Ypos + Line_width, Height - Line_width);
    LCD_DrawHLine(Xpos + Line_width, Ypos + Height - Line_width, Width - Line_width + 1);
    LCD_DrawVLine(Xpos + Width - Line_width, Ypos + Line_width, Height - Line_width);
  }

  // LCD_DrawHLine(Xpos, Ypos, Width);
  // LCD_DrawVLine(Xpos, Ypos, Height);
  // LCD_DrawHLine(Xpos, Ypos + Height, Width + 1);
  // LCD_DrawVLine(Xpos + Width, Ypos, Height);
}



/**
 * @brief 填充一个缓冲区
 * 
 * @param LayerIndex 1：前景层   0：背景层
 * @param pDst 指向目标缓冲区的指针
 * @param xSize 缓冲区宽度
 * @param ySize 缓冲区高度
 * @param OffLine 偏移量
 * @param ColorIndex 填充颜色
 */
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
  HAL_DMA2D_DeInit(&DMA2D_Handle);
  
  DMA2D_Handle.Init.Mode = DMA2D_R2M;
	
  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_RGB565:DMA2D_Handle.Init.ColorMode = DMA2D_RGB565;break;
    case LTDC_PIXEL_FORMAT_ARGB8888:DMA2D_Handle.Init.ColorMode = DMA2D_ARGB8888;break;
    case LTDC_PIXEL_FORMAT_RGB888:DMA2D_Handle.Init.ColorMode = DMA2D_RGB888;break;
    case LTDC_PIXEL_FORMAT_ARGB1555:DMA2D_Handle.Init.ColorMode = DMA2D_ARGB1555;break;
    case LTDC_PIXEL_FORMAT_ARGB4444:DMA2D_Handle.Init.ColorMode = DMA2D_ARGB4444;break;
  }
	
  DMA2D_Handle.Init.OutputOffset = OffLine;
	
	DMA2D_Handle.Instance = DMA2D;

  if(HAL_DMA2D_Init(&DMA2D_Handle) == HAL_OK)   //DMA2D初始化
  {
    if(HAL_DMA2D_ConfigLayer(&DMA2D_Handle, LayerIndex) == HAL_OK)
    {
      if(HAL_DMA2D_Start(&DMA2D_Handle, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        HAL_DMA2D_PollForTransfer(&DMA2D_Handle, 100);   //DMA轮询传输
      }
    }
  }
}

/**
 * @brief 复制绘图缓冲区数据到显存
 * 
 * @param X1pos 填充区域起始位置X坐标
 * @param Y1pos 填充区域起始位置Y坐标
 * @param X2pos 填充区域结束位置X坐标
 * @param Y2pos 填充区域结束位置Y坐标
 * @param pColor 填充的颜色数组指针
 */
void FillColorBuffer(uint16_t X1pos, uint16_t Y1pos, uint16_t X2pos, uint16_t Y2pos, uint32_t * pColor)
{
  uint32_t Width = (X2pos > X1pos) ? (X2pos - X1pos) : (X1pos - X2pos);
  uint32_t Height = (Y2pos > Y1pos) ? (Y2pos - Y1pos) : (Y1pos - Y2pos);
  uint32_t offset = LCD_GetXSize() - (Width + 1);
  uint32_t address = LCD_GetStartAddress(X1pos, Y1pos);

  HAL_DMA2D_DeInit(&DMA2D_Handle);

  DMA2D_Handle.Instance = DMA2D;

  DMA2D_Handle.Init.Mode = DMA2D_M2M;
  DMA2D_Handle.Init.ColorMode = DMA2D_ARGB8888;
  DMA2D_Handle.Init.OutputOffset = offset;

  if(HAL_DMA2D_Init(&DMA2D_Handle) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 1) == HAL_OK)
    {
      if(HAL_DMA2D_Start(&DMA2D_Handle, (uint32_t)pColor, address, Width, Height) == HAL_OK)
      {
        HAL_DMA2D_PollForTransfer(&DMA2D_Handle, 200);
      }
    }
  }
}

/**
 * @brief 绘制一个点
 * 
 * @param Xpos X轴坐标
 * @param Ypos Y轴坐标
 * @param RGB_Code 像素颜色值
 */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:  //ARGB8888格式占用4个字节，要绘制的像素点地址等于(显存首地址 + 偏移像素个数 * 4字节)，其他格式同理
           *(__IO uint32_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (4 * (Ypos * LCD_GetXSize() + Xpos))) = RGB_Code;
           break;
    case LTDC_PIXEL_FORMAT_RGB888:
           *(__IO uint32_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (3 * (Ypos * LCD_GetXSize() + Xpos)) + 2) = 0xFF & (RGB_Code>>16);
           *(__IO uint32_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (3 * (Ypos * LCD_GetXSize() + Xpos)) + 1) = 0xFF & (RGB_Code>>8);
           *(__IO uint32_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (3 * (Ypos * LCD_GetXSize() + Xpos)) + 0) = 0xFF & (RGB_Code>>0);
           break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:
           *(__IO uint32_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;
           break;
    default:
           *(__IO uint32_t*)(LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;
           break;
  }
}

/**
 * @brief 绘制一条水平直线
 * 
 * @param Xpos X轴起始坐标
 * @param Ypos Y轴起始坐标
 * @param Length 线的长度
 */
void LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t StartAddress = 0;
  uint8_t temp = 1;

  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:temp = 4;break;
    case LTDC_PIXEL_FORMAT_RGB888:temp = 3;break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:temp = 2;break;
    default:temp = 2;break;
  }

  StartAddress = LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (temp * (Ypos * LCD_GetXSize() + Xpos));

  LL_FillBuffer(ActiveLayer, (uint32_t *)StartAddress, Length, 1, 0, DrawProp[ActiveLayer].TextColor);
}

/**
 * @brief 绘制一条垂直直线
 * 
 * @param Xpos X轴起始坐标
 * @param Ypos Y轴起始坐标
 * @param Length 线的长度
 */
void LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t StartAddress = 0;
  uint8_t temp = 1;

  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:temp = 4;break;
    case LTDC_PIXEL_FORMAT_RGB888:temp = 3;break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:temp = 2;break;
    default:temp = 2;break;
  }

  StartAddress = LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (temp * (Ypos * LCD_GetXSize() + Xpos));

  LL_FillBuffer(ActiveLayer, (uint32_t *)StartAddress, 1, Length, (LCD_GetXSize() - 1), DrawProp[ActiveLayer].TextColor);
}


/**
 * @brief 绘制任意两点之间的一条直线
 * 
 * @param X1pos 点1的X坐标
 * @param Y1pos 点1的Y坐标
 * @param X2pos 点2的X坐标
 * @param Y2pos 点2的Y坐标
 */
void LCD_DrawAnyLine(uint16_t X1pos, uint16_t Y1pos, uint16_t X2pos, uint16_t Y2pos)
{
  float a, b;
  uint16_t x, y, i;
  uint16_t temp = 0;

  //先判断两点之间的线是否是水平线或垂直线，是的话直接调用画直线的函数，然后退出
  if(X1pos == X2pos)   //x坐标相等，说明是一条垂直的线
  {
    if(Y2pos >= Y1pos)  //点2在下面，从点1开始从上往下画
    {
      LCD_DrawVLine(X1pos, Y1pos, Y2pos - Y1pos);
    }
    else   //点1在下面，从点2开始从上往下画
    {
      LCD_DrawVLine(X2pos, Y2pos, Y1pos - Y2pos);
    }
    return;
  }

  if(Y1pos == Y2pos)   //y坐标相等，说明是一条水平的线
  {
    if(X2pos >= X1pos)   //点2在右边，从点1开始从左往右画
    {
      LCD_DrawHLine(X1pos, Y1pos, X2pos - X1pos);
    }
    else   //点1在右边，从点2开始从左往右画
    {
      LCD_DrawHLine(X2pos, Y2pos, X1pos - X2pos);
    }
    return;
  }

  //不是水平线或垂直线

  //先将两个点画出来
  LCD_DrawPixel(X1pos, Y1pos, DrawProp[ActiveLayer].TextColor);
  LCD_DrawPixel(X2pos, Y2pos, DrawProp[ActiveLayer].TextColor);

  //判断两个点的左右位置，然后进行调换，保证点1在左边，点2在右边
  if(X1pos > X2pos)
  {
    //更换x轴位置
    temp = X1pos;
    X1pos = X2pos;
    X2pos = temp;

    //更换y轴位置
    temp = Y1pos;
    Y1pos = Y2pos;
    Y2pos = temp;
  }

  //计算正比例函数(y = ax + b)中的参数a和b
  a = ((float)Y1pos - (float)Y2pos) / ((float)X1pos - (float)X2pos);
  b = (float)Y1pos - a * (float)X1pos;

  //考虑到如果比例太大(a太大)，会导致计算出的y值很大，可能为无限大，所以分成两种函数执行，分别是y = ax + b 和 x = ay + b;
  if((a <= 1) && (a >= -1))   // y = ax + b
  {
    for(i = 1; i < (X2pos - X1pos); i++)
    {
      x = X1pos + i;
      y = a * (float)x + b;
      LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);
    }
  }
  else   // x = ay + b
  {
    a = ((float)X1pos - (float)X2pos) / ((float)Y1pos - (float)Y2pos);
    b = (float)X1pos - a * (float)Y1pos;

    if(Y2pos > Y1pos)
    {
      for(i = 1; i < (Y2pos - Y1pos); i++)
      {
        y = Y1pos + i;
        x = a * (float)y + b;
        LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);
      }
    }
    else
    {
      for(i = 1; i < (Y1pos - Y2pos); i++)
      {
        y = Y2pos + i;
        x = a * (float)y + b;
        LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);
      }
    }
  }
}

void LCD_DisplayOn(void)
{
  __HAL_LTDC_ENABLE(&LTDC_Handle);
  HAL_GPIO_WritePin(LTDC_DISP_GPIO_PORT, LTDC_DISP_GPIO_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_Pin, GPIO_PIN_SET);
}

void LCD_DisplayOff(void)
{
  __HAL_LTDC_DISABLE(&LTDC_Handle);
  HAL_GPIO_WritePin(LTDC_DISP_GPIO_PORT, LTDC_DISP_GPIO_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_Pin, GPIO_PIN_RESET);
}

uint32_t LCD_GetStartAddress(uint16_t Xpos, uint16_t Ypos)
{
  uint32_t StartAddress = 0;
  uint8_t temp = 1;

  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:temp = 4;break;
    case LTDC_PIXEL_FORMAT_RGB888:temp = 3;break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:temp = 2;break;
    default:temp = 2;break;
  }

  StartAddress = LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress + (temp * (Ypos * LCD_GetXSize() + Xpos));

  return StartAddress;
}

/**
 * @brief 绘制一个字符
 * 
 * @param Xpos X坐标
 * @param Ypos Y坐标
 * @param c 字符
 */
static void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint8_t temp = (0x01 << 7);   //用于逐位检测是否为1，在字节里面必须从左往右检测，因为绘制像素点时是从左往右绘制的
  uint16_t i, j;
	uint8_t Width_Byte, Height_Pixel;

  Width_Byte = DrawProp[ActiveLayer].pFont->Width;   //宽度用字节表示
  Height_Pixel = DrawProp[ActiveLayer].pFont->Height;   //高度用像素行表示

  if((Width_Byte % 8) != 0)
  {
    Width_Byte = Width_Byte / 8 + 1;
  }
  else
  {
    Width_Byte = Width_Byte / 8;
  }

  for(i = 0; i < (Width_Byte * Height_Pixel); i++)   //遍历每一个字节
  {
    for(j = 0; j < 8; j++)   //遍历字节里面的每一个位
    {
      if((*c & (temp >> j)))   //检查*c里面的每一个位，如果该位为1，则绘制一个像素点
      {
        //X轴偏移：起始位置 + 字节偏移 + 位偏移    Y轴偏移：起始位置 + 行偏移
        LCD_DrawPixel((Xpos + (8 * (i % Width_Byte)) + j), (Ypos + i / Width_Byte), DrawProp[ActiveLayer].TextColor);
      }
    }
		c++;   //将指针指向下一个字节
  }
}

/**
 * @brief 在指定位置显示一个字符(英文)
 * 
 * @param Xpos 显示的X坐标
 * @param Ypos 显示的Y坐标
 * @param Ascii 需要显示的字符
 */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
  uint8_t Width_Byte, Height_Pixel;

  Width_Byte = DrawProp[ActiveLayer].pFont->Width;   //宽度用字节表示
  Height_Pixel = DrawProp[ActiveLayer].pFont->Height;   //高度用像素行表示

  if((Width_Byte % 8) != 0)
  {
    Width_Byte = Width_Byte / 8 + 1;
  }
  else
  {
    Width_Byte = Width_Byte / 8;
  }

  LCD_DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii - ' ') * Width_Byte * Height_Pixel]);
}

/**
 * @brief 在指定位置显示一个字符串(英文)
 * @note 位置不够显示一个字符时自动换行
 * @param Xpos 显示的起始位置X坐标
 * @param Ypos 显示的起始位置Y坐标
 * @param Text 指向字符串的指针
 * @return pos_t 返回显示完字符串后的指针位置
 */
pos_t LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t * Text)
{
  uint16_t i = 0;
  uint32_t strNum;
  pos_t newpos;

  strNum = strlen((const char *)Text);

  while(i < strNum)
  {
    if(*Text != '\n')
    {
      LCD_DisplayChar(Xpos, Ypos, *Text);
      Xpos += DrawProp[ActiveLayer].pFont->Width;
      if((LCD_GetXSize() - Xpos) < DrawProp[ActiveLayer].pFont->Width)   //剩余位置不够显示一个字符，自动换行
      {
        Ypos += DrawProp[ActiveLayer].pFont->Height;
        Xpos = 0;
      }
    }
    else   //遇到换行符进行换行
    {
      Ypos += DrawProp[ActiveLayer].pFont->Height;
      Xpos = 0;
    }

    //剩余高度不够写下一行时，将屏幕向上滚动一行，并将指针高度回退到最后一行
    if((LCD_GetYSize() - Ypos) < DrawProp[ActiveLayer].pFont->Height)
    {
      LCD_Scroll(DrawProp[ActiveLayer].pFont->Height);   //滚动屏幕
      Ypos -= DrawProp[ActiveLayer].pFont->Height;   //将指针高度回退到最后一行
    }

    i++;
    Text++;
  }

  newpos.Xpos = Xpos;
  newpos.Ypos = Ypos;

  return newpos;
}

/**
 * @brief 在指定行显示一个字符串(英文)
 * 
 * @param Line 显示的起始行(超出行宽会自动换行)
 * @param Text 指向字符串的指针
 */
void LCD_DisplayStringLine(uint8_t Line, uint8_t * Text)
{
  LCD_DisplayStringAt(0, LINE(Line), Text);
}

/**
 * @brief 绘制一个中文字符
 * 
 * @param Xpos 绘制字符的X坐标
 * @param Ypos 绘制字符的Y坐标
 * @param char_CH 中文字符
 */
void LCD_DisplayChar_CH(uint16_t Xpos, uint16_t Ypos, uint16_t char_CH)
{
  uint8_t i, j;
  uint8_t temp = (0x01 << 7);   //用于逐位检测是否为1，在字节里面必须从左往右检测，因为绘制像素点时是从左往右绘制的
  uint8_t charBuffer[WIDTH_CH_CHAR * HEIGHT_CH_CHAR / 8];
  uint8_t Width_Byte, Height_Pixel;

  GetGB2312Code_from_EXFlash(charBuffer, char_CH);   //获取字符的字模数据

  //以下部分和英文字符绘制函数类似
  if((WIDTH_CH_CHAR % 8) != 0)
  {
    Width_Byte = WIDTH_CH_CHAR / 8 + 1;
  }
  else
  {
    Width_Byte = WIDTH_CH_CHAR / 8;
  }
  Height_Pixel = HEIGHT_CH_CHAR;


  for(i = 0; i < (Width_Byte * Height_Pixel); i++)
  {
    for(j = 0; j < 8; j++)
    {
      if(charBuffer[i] & (temp >> j))
      {
        LCD_DrawPixel((Xpos + (8 * (i % Width_Byte)) + j), (Ypos + i / Width_Byte), DrawProp[ActiveLayer].TextColor);
      }
//      else   //开启非透明显示，即在字符区域内的非笔画区域填充背景色
//      {
//        LCD_DrawPixel((Xpos + (8 * (i % Width_Byte)) + j), (Ypos + i / Width_Byte), DrawProp[ActiveLayer].BackColor);
//      }
    }
  }
}

void LCD_DisplayStringAt_EN_CH(uint16_t Xpos, uint16_t Ypos, uint8_t *Text)
{
  uint16_t char_CH;
	uint16_t temp = Xpos;

  while(*Text != 0)
  {
    if(*Text < 127)   //编码小于127的字符为英文字符
    {
      LCD_DisplayChar(Xpos, Ypos, *(uint8_t *)Text);
      Xpos += DrawProp[ActiveLayer].pFont->Width;
      Text++;   //指向下一个字符，英文字符占一个字节
    }
    else   //编码大于127则为中文字符
    {
      char_CH = ((*(uint16_t *)Text) >> 8) + ((*(uint16_t *)Text) << 8);   //由于中文字符在STM32里面是按小端模式存放的，所以需要调换位置变成大端模式
      LCD_DisplayChar_CH(Xpos, Ypos, char_CH);
      Xpos += WIDTH_CH_CHAR;
      Text += 2;   //指向下一个字符，中文字符占两个字节
    }

    if((LCD_GetXSize() - Xpos) < WIDTH_CH_CHAR)   //行剩余位置不足以显示一个中文字符，则进行换行
    {
      Xpos = temp;
      Ypos += (((sFONT *)LCD_GetFont())->Height);
    }
  }
}

/**
 * @brief 显示一句可以包含中英文的句子
 * 
 * @param Line 显示的起始行(超出行宽会自动换行)
 * @param Text 指向需要显示的句子的指针
 */
void LCD_DisplayStringLine_EN_CH(uint16_t Line, uint8_t *Text)
{
  uint16_t Xpos = 0;
  uint16_t Ypos = LINE(Line);
  uint16_t char_CH;

  while(*Text != 0)
  {
    if(*Text < 127)   //编码小于127的字符为英文字符
    {
      LCD_DisplayChar(Xpos, Ypos, *(uint8_t *)Text);
      Xpos += DrawProp[ActiveLayer].pFont->Width;
      Text++;   //指向下一个字符，英文字符占一个字节
    }
    else   //编码大于127则为中文字符
    {
      char_CH = ((*(uint16_t *)Text) >> 8) + ((*(uint16_t *)Text) << 8);   //由于中文字符在STM32里面是按小端模式存放的，所以需要调换位置变成大端模式
      LCD_DisplayChar_CH(Xpos, Ypos, char_CH);
      Xpos += WIDTH_CH_CHAR;
      Text += 2;   //指向下一个字符，中文字符占两个字节
    }

    if((LCD_GetXSize() - Xpos) < WIDTH_CH_CHAR)   //行剩余位置不足以显示一个中文字符，则进行换行
    {
      Xpos = 0;
      Ypos = LINE(++Line);
    }
  }
}

/**
 * @brief 获取一个中文GB2312编码字符的字模数据
 * 
 * @param pBuffer 用于存放字模数据的缓冲区
 * @param c GB2312编码字符
 * @return int 无意义
 */
static int GetGB2312Code_from_EXFlash(uint8_t * pBuffer, uint16_t c)
{
  unsigned char High8bit, Low8bit;
  unsigned int pos;

  High8bit = c >> 8;   //取字符编码的高八位
  Low8bit = c & 0xFF;   //取字符编码的低八位

  //计算字模在字库中的偏移位置
  pos = ((High8bit - 0xA1) * 94 + (Low8bit - 0xA1)) * (WIDTH_CH_CHAR * HEIGHT_CH_CHAR / 8);   //计算公式：偏移的字模个数 * 单个字模所占字节

  spi_flash_BufferRead((GB2312CODE_START_ADDRESS + pos), pBuffer, (WIDTH_CH_CHAR * HEIGHT_CH_CHAR / 8));

  return 0;
}

/**
 * @brief 在LCD上进行printf输出
 * 
 * @param text 参数格式和printf一样
 * @param ... 参数格式和printf一样
 */
void LCD_printf(char * text, ...)
{
  uint16_t strLenMax = LCD_PIXEL_WIDTH / DrawProp[ActiveLayer].pFont->Width;   //一整行字符的字节数
  uint8_t buffer[strLenMax * 3];   //为了防止数组溢出，数组大小设定为3行字符的大小
  static pos_t printf_pos = {.Xpos = 0, .Ypos = 0};   //位置指针
  pos_t newpos;
  va_list p_arg;

  va_start(p_arg, text);
  vsnprintf((char *)buffer, sizeof(buffer), (const char *)text, p_arg);   //将输入参数格式化后保存到buffer变量里面
  va_end(p_arg);

  newpos = LCD_DisplayStringAt(printf_pos.Xpos, printf_pos.Ypos, buffer);   //在LCD上显示格式化后的字符串

  printf_pos = newpos;   //保存位置指针，以便下一次调用
}

/**
 * @brief 屏幕滚动
 * 
 * @param line_num 滚动的行数(像素行)
 */
void LCD_Scroll(uint16_t line_num)
{
  uint32_t destin = LTDC_Handle.LayerCfg[ActiveLayer].FBStartAdress;
  uint32_t source;
  uint8_t temp = 1;

  switch(LTDC_Handle.LayerCfg[ActiveLayer].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:temp = 4;break;
    case LTDC_PIXEL_FORMAT_RGB888:temp = 3;break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:temp = 2;break;
    default:temp = 2;break;
  }

  source = destin + temp * LCD_GetXSize() * line_num;

  memcpy((void *)destin, (void *)source, temp * (LCD_GetXSize() * (LCD_GetYSize() - line_num)));

  LL_FillBuffer(ActiveLayer, (uint32_t *)(destin + temp * (480 - line_num) * LCD_GetXSize()), 800, line_num, 0, DrawProp[ActiveLayer].BackColor);
}
