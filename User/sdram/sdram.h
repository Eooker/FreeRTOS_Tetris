#ifndef __SDRAM_H
#define __SDRAM_H

#include "stm32f4xx.h"

#define SDRAM_ADDRESS   ((uint32_t)0xD0000000)

/*SDRAM模式寄存器配置*/
//突发长度
#define SDRAM_MODEREG_BURST_LENGTH_1              ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2              ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4              ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8              ((uint16_t)0x0004)
//突发方式为顺序突发
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL       ((uint16_t)0x0000)
//突发方式为交错突发
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED      ((uint16_t)0x0008)
//读取命令到输出数据之间的延迟，单位为时钟周期
#define SDRAM_MODEREG_CAS_LATENCY_2               ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3               ((uint16_t)0x0030)
//工作模式为正常模式，其他值为测试模式或保留的设定
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD     ((uint16_t)0x0000)
//写入突发长度为设定值
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED  ((uint16_t)0x0000)
//写入突发长度为1，即不启用突发写入
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE      ((uint16_t)0x0200)

/*地址信号线*/
#define FMC_A0_GPIO_PORT        GPIOF
#define FMC_A0_GPIO_PIN         GPIO_PIN_0

#define FMC_A1_GPIO_PORT        GPIOF
#define FMC_A1_GPIO_PIN         GPIO_PIN_1

#define FMC_A2_GPIO_PORT        GPIOF
#define FMC_A2_GPIO_PIN         GPIO_PIN_2

#define FMC_A3_GPIO_PORT        GPIOF
#define FMC_A3_GPIO_PIN         GPIO_PIN_3

#define FMC_A4_GPIO_PORT        GPIOF
#define FMC_A4_GPIO_PIN         GPIO_PIN_4

#define FMC_A5_GPIO_PORT        GPIOF
#define FMC_A5_GPIO_PIN         GPIO_PIN_5

#define FMC_A6_GPIO_PORT        GPIOF
#define FMC_A6_GPIO_PIN         GPIO_PIN_12

#define FMC_A7_GPIO_PORT        GPIOF
#define FMC_A7_GPIO_PIN         GPIO_PIN_13

#define FMC_A8_GPIO_PORT        GPIOF
#define FMC_A8_GPIO_PIN         GPIO_PIN_14

#define FMC_A9_GPIO_PORT        GPIOF
#define FMC_A9_GPIO_PIN         GPIO_PIN_15

#define FMC_A10_GPIO_PORT        GPIOG
#define FMC_A10_GPIO_PIN         GPIO_PIN_0

#define FMC_A11_GPIO_PORT        GPIOG
#define FMC_A11_GPIO_PIN         GPIO_PIN_1

#define FMC_ADDRESS_WIRE_GPIO_CLK_EN     {__HAL_RCC_GPIOF_CLK_ENABLE();__HAL_RCC_GPIOG_CLK_ENABLE();}


/*数据信号线*/
#define FMC_D0_GPIO_PORT        GPIOD
#define FMC_D0_GPIO_PIN         GPIO_PIN_14

#define FMC_D1_GPIO_PORT        GPIOD
#define FMC_D1_GPIO_PIN         GPIO_PIN_15

#define FMC_D2_GPIO_PORT        GPIOD
#define FMC_D2_GPIO_PIN         GPIO_PIN_0

#define FMC_D3_GPIO_PORT        GPIOD
#define FMC_D3_GPIO_PIN         GPIO_PIN_1

#define FMC_D4_GPIO_PORT        GPIOE
#define FMC_D4_GPIO_PIN         GPIO_PIN_7

#define FMC_D5_GPIO_PORT        GPIOE
#define FMC_D5_GPIO_PIN         GPIO_PIN_8

#define FMC_D6_GPIO_PORT        GPIOE
#define FMC_D6_GPIO_PIN         GPIO_PIN_9

#define FMC_D7_GPIO_PORT        GPIOE
#define FMC_D7_GPIO_PIN         GPIO_PIN_10

#define FMC_D8_GPIO_PORT        GPIOE
#define FMC_D8_GPIO_PIN         GPIO_PIN_11

#define FMC_D9_GPIO_PORT        GPIOE
#define FMC_D9_GPIO_PIN         GPIO_PIN_12

#define FMC_D10_GPIO_PORT        GPIOE
#define FMC_D10_GPIO_PIN         GPIO_PIN_13

#define FMC_D11_GPIO_PORT        GPIOE
#define FMC_D11_GPIO_PIN         GPIO_PIN_14

#define FMC_D12_GPIO_PORT        GPIOE
#define FMC_D12_GPIO_PIN         GPIO_PIN_15

#define FMC_D13_GPIO_PORT        GPIOD
#define FMC_D13_GPIO_PIN         GPIO_PIN_8

#define FMC_D14_GPIO_PORT        GPIOD
#define FMC_D14_GPIO_PIN         GPIO_PIN_9

#define FMC_D15_GPIO_PORT        GPIOD
#define FMC_D15_GPIO_PIN         GPIO_PIN_10

#define FMC_DATA_WIRE_GPIO_CLK_EN     {__HAL_RCC_GPIOD_CLK_ENABLE();__HAL_RCC_GPIOE_CLK_ENABLE();}



/*控制信号线*/
#define FMC_CS_GPIO_PORT        GPIOH    //片选信号，低电平有效
#define FMC_CS_GPIO_PIN         GPIO_PIN_6

#define FMC_BA0_GPIO_PORT        GPIOG   //SDRAM芯片的Bank选择，用于选择要控制的Bank
#define FMC_BA0_GPIO_PIN         GPIO_PIN_4

#define FMC_BA1_GPIO_PORT        GPIOG   //SDRAM芯片的Bank选择，用于选择要控制的Bank
#define FMC_BA1_GPIO_PIN         GPIO_PIN_5

#define FMC_WE_GPIO_PORT        GPIOC    //写入使能，低电平有效
#define FMC_WE_GPIO_PIN         GPIO_PIN_0

#define FMC_RAS_GPIO_PORT        GPIOF   //行地址选通，低电平有效，选通时地址线表示行地址
#define FMC_RAS_GPIO_PIN         GPIO_PIN_11

#define FMC_CAS_GPIO_PORT        GPIOG   //列地址选通，低电平有效，选通时地址线表示行地址
#define FMC_CAS_GPIO_PIN         GPIO_PIN_15

#define FMC_CLK_GPIO_PORT        GPIOG   //同步时钟信号，在CLK上升沿时采集输入信号
#define FMC_CLK_GPIO_PIN         GPIO_PIN_8

#define FMC_CKE_GPIO_PORT        GPIOH   //时钟使能信号，禁止时钟信号时SDRAM会启动自刷新操作
#define FMC_CKE_GPIO_PIN         GPIO_PIN_7

#define FMC_UDQM_GPIO_PORT        GPIOE  //数据掩码信号，用于选择数据信号线的有效位
#define FMC_UDQM_GPIO_PIN         GPIO_PIN_1

#define FMC_LDQM_GPIO_PORT        GPIOE  //数据掩码信号，用于选择数据信号线的有效位
#define FMC_LDQM_GPIO_PIN         GPIO_PIN_0

#define FMC_CONTROL_WIRE_GPIO_CLK_EN     {__HAL_RCC_GPIOH_CLK_ENABLE();__HAL_RCC_GPIOG_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOF_CLK_ENABLE();__HAL_RCC_GPIOE_CLK_ENABLE();}

void SDRAM_Config(void);

#endif
