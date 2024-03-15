#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include "stm32f4xx.h"
#include "fonts.h"

#define LTDC_R0_GPIO_PORT GPIOH
#define LTDC_R0_GPIO_Pin GPIO_PIN_2

#define LTDC_R1_GPIO_PORT GPIOH
#define LTDC_R1_GPIO_Pin GPIO_PIN_3

#define LTDC_R2_GPIO_PORT GPIOH
#define LTDC_R2_GPIO_Pin GPIO_PIN_8

#define LTDC_R3_GPIO_PORT GPIOB
#define LTDC_R3_GPIO_Pin GPIO_PIN_0

#define LTDC_R4_GPIO_PORT GPIOA
#define LTDC_R4_GPIO_Pin GPIO_PIN_11

#define LTDC_R5_GPIO_PORT GPIOA
#define LTDC_R5_GPIO_Pin GPIO_PIN_12

#define LTDC_R6_GPIO_PORT GPIOB
#define LTDC_R6_GPIO_Pin GPIO_PIN_1

#define LTDC_R7_GPIO_PORT GPIOG
#define LTDC_R7_GPIO_Pin GPIO_PIN_6


#define LTDC_G0_GPIO_PORT GPIOE
#define LTDC_G0_GPIO_Pin GPIO_PIN_5

#define LTDC_G1_GPIO_PORT GPIOE
#define LTDC_G1_GPIO_Pin GPIO_PIN_6

#define LTDC_G2_GPIO_PORT GPIOH
#define LTDC_G2_GPIO_Pin GPIO_PIN_13

#define LTDC_G3_GPIO_PORT GPIOG
#define LTDC_G3_GPIO_Pin GPIO_PIN_10

#define LTDC_G4_GPIO_PORT GPIOH
#define LTDC_G4_GPIO_Pin GPIO_PIN_15

#define LTDC_G5_GPIO_PORT GPIOI
#define LTDC_G5_GPIO_Pin GPIO_PIN_0

#define LTDC_G6_GPIO_PORT GPIOC
#define LTDC_G6_GPIO_Pin GPIO_PIN_7

#define LTDC_G7_GPIO_PORT GPIOI
#define LTDC_G7_GPIO_Pin GPIO_PIN_2


#define LTDC_B0_GPIO_PORT GPIOE
#define LTDC_B0_GPIO_Pin GPIO_PIN_4

#define LTDC_B1_GPIO_PORT GPIOG
#define LTDC_B1_GPIO_Pin GPIO_PIN_12

#define LTDC_B2_GPIO_PORT GPIOD
#define LTDC_B2_GPIO_Pin GPIO_PIN_6

#define LTDC_B3_GPIO_PORT GPIOG
#define LTDC_B3_GPIO_Pin GPIO_PIN_11

#define LTDC_B4_GPIO_PORT GPIOI
#define LTDC_B4_GPIO_Pin GPIO_PIN_4

#define LTDC_B5_GPIO_PORT GPIOA
#define LTDC_B5_GPIO_Pin GPIO_PIN_3

#define LTDC_B6_GPIO_PORT GPIOB
#define LTDC_B6_GPIO_Pin GPIO_PIN_8

#define LTDC_B7_GPIO_PORT GPIOB
#define LTDC_B7_GPIO_Pin GPIO_PIN_9


#define LTDC_CLK_GPIO_PORT GPIOG
#define LTDC_CLK_GPIO_Pin GPIO_PIN_7

#define LTDC_HSYNC_GPIO_PORT GPIOI
#define LTDC_HSYNC_GPIO_Pin GPIO_PIN_10

#define LTDC_VSYNC_GPIO_PORT GPIOI
#define LTDC_VSYNC_GPIO_Pin GPIO_PIN_9


#define LTDC_DE_GPIO_PORT GPIOF
#define LTDC_DE_GPIO_Pin GPIO_PIN_10

#define LTDC_DISP_GPIO_PORT GPIOD
#define LTDC_DISP_GPIO_Pin GPIO_PIN_4

#define LTDC_BL_GPIO_PORT GPIOD
#define LTDC_BL_GPIO_Pin GPIO_PIN_7

#define LTDC_GPIO_CLK_ENABLE()   do{__GPIOA_CLK_ENABLE();__GPIOB_CLK_ENABLE();__GPIOC_CLK_ENABLE();__GPIOD_CLK_ENABLE();__GPIOE_CLK_ENABLE();\
                                 __GPIOF_CLK_ENABLE();__GPIOG_CLK_ENABLE();__GPIOH_CLK_ENABLE();__GPIOI_CLK_ENABLE();}while(0)

#define LCD_COLOR_BLUE          ((uint32_t)0xFF0000FF)
#define LCD_COLOR_GREEN         ((uint32_t)0xFF00FF00)
#define LCD_COLOR_RED           ((uint32_t)0xFFFF0000)
#define LCD_COLOR_CYAN          ((uint32_t)0xFF00FFFF)
#define LCD_COLOR_MAGENTA       ((uint32_t)0xFFFF00FF)
#define LCD_COLOR_YELLOW        ((uint32_t)0xFFFFFF00)
#define LCD_COLOR_LIGHTBLUE     ((uint32_t)0xFF8080FF)
#define LCD_COLOR_LIGHTGREEN    ((uint32_t)0xFF80FF80)
#define LCD_COLOR_LIGHTRED      ((uint32_t)0xFFFF8080)
#define LCD_COLOR_LIGHTCYAN     ((uint32_t)0xFF80FFFF)
#define LCD_COLOR_LIGHTMAGENTA  ((uint32_t)0xFFFF80FF)
#define LCD_COLOR_LIGHTYELLOW   ((uint32_t)0xFFFFFF80)
#define LCD_COLOR_DARKBLUE      ((uint32_t)0xFF000080)
#define LCD_COLOR_DARKGREEN     ((uint32_t)0xFF008000)
#define LCD_COLOR_DARKRED       ((uint32_t)0xFF800000)
#define LCD_COLOR_DARKCYAN      ((uint32_t)0xFF008080)
#define LCD_COLOR_DARKMAGENTA   ((uint32_t)0xFF800080)
#define LCD_COLOR_DARKYELLOW    ((uint32_t)0xFF808000)
#define LCD_COLOR_WHITE         ((uint32_t)0xFFFFFFFF)
#define LCD_COLOR_LIGHTGRAY     ((uint32_t)0xFFD3D3D3)
#define LCD_COLOR_GRAY          ((uint32_t)0xFF808080)
#define LCD_COLOR_DARKGRAY      ((uint32_t)0xFF404040)
#define LCD_COLOR_BLACK         ((uint32_t)0xFF000000)
#define LCD_COLOR_BROWN         ((uint32_t)0xFFA52A2A)
#define LCD_COLOR_ORANGE        ((uint32_t)0xFFFFA500)
#define LCD_COLOR_TRANSPARENT   ((uint32_t)0xFF000000)
#define LCD_COLOR_FUCHSIA       ((uint32_t)0xFFFF00FF)   //紫红色
#define LCD_COLOR_VIOLET        ((uint32_t)0xFFEEB2EE)   //紫罗兰色
#define LCD_COLOR_HOTPINK       ((uint32_t)0xFFFF69B4)   //热情的粉红色
#define LCD_COLOR_DEEPPINK      ((uint32_t)0xFFFF1493)   //深粉红色
#define LCD_COLOR_GOLD          ((uint32_t)0xFFFFD700)   //金色


//LCD默认字体
#define LCD_DEFAULT_FONT Font24

//LCD物理像素大小(宽度和高度)
#define LCD_PIXEL_WIDTH   (uint16_t)800
#define LCD_PIXEL_HEIGHT  (uint16_t)480

//根据液晶数据手册的参数配置
//HSYNC后的无效像素
#define HBP 46
//VSYNC后的无效行数
#define VBP 23
//HSYNC宽度
#define HSW 1
//VSYNC宽度
#define VSW 1
//HSYNC前的无效像素
#define HFP 22
//VSYNC前的无效行数
#define VFP 22

#define MAX_LAYER_NUMBER   (uint32_t)2
#define LTDC_ACTIVE_LAYER  (uint32_t)1

#define LCD_FB_START_ADDRESS   (uint32_t)0xD0000000

typedef enum
{
  INCH_5 = 0x00,   //5寸屏
  INCH_7,   //7寸屏
  INCH_4_3,   //4.3寸屏

  LCD_TYPE_NUM   //LCD类型总数
} LCD_TypeDef;
extern LCD_TypeDef cur_lcd;

typedef struct
{
  uint32_t TextColor;
  uint32_t BackColor;
  sFONT *pFont;   //注意这里是一个指针，使用这个成员内的成员(sFONT也是结构体)的时候应该用->，如AAA.pFont->Width
} LCD_DrawPropTypeDef;

typedef struct
{
  uint16_t Xpos;
  uint16_t Ypos;
} pos_t;


//LCD配置函数
void LCD_Config(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
uint32_t LCD_GetXSize(void);
uint32_t LCD_GetYSize(void);
void LCD_SetXSize(uint32_t imageWidthPixels);
void LCD_SetYSize(uint32_t imageHeightPixels);
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address, uint32_t PixelFormat);
uint32_t LCD_GetStartAddress(uint16_t Xpos, uint16_t Ypos);

//LCD显示配置函数
void LCD_SelectLayer(uint32_t LayerIndex);
void LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State);
void LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency);
void LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address);
void LCD_Clear(uint32_t Color);
void LCD_ClearLine(uint32_t Line);
uint32_t LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos);
void LCD_SetTextColor(uint32_t Color);
void LCD_SetBackColor(uint32_t Color);
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void LCD_Scroll(uint16_t line_num);

//LCD图形显示函数
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code);
void LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void LCD_DrawAnyLine(uint16_t X1pos, uint16_t Y1pos, uint16_t X2pos, uint16_t Y2pos);
void LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint32_t Color);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint8_t Line_width);
void FillColorBuffer(uint16_t X1pos, uint16_t Y1pos, uint16_t X2pos, uint16_t Y2pos, uint32_t * pColor);

//LCD字符显示函数
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);
pos_t LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t * Text);
void LCD_DisplayStringLine(uint8_t Line, uint8_t * Text);
void LCD_DisplayChar_CH(uint16_t Xpos, uint16_t Ypos, uint16_t char_CH);
void LCD_DisplayStringAt_EN_CH(uint16_t Xpos, uint16_t Ypos, uint8_t *Text);
void LCD_DisplayStringLine_EN_CH(uint16_t Line, uint8_t *Text);
void LCD_printf(char * text, ...);

#endif
