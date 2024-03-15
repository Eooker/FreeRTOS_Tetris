#include "sdram.h"

SDRAM_HandleTypeDef SDRAM_Handle;

static void SDRAM_GPIO_Config(void);
static void SDRAM_InitSequence(void);

static void SDRAM_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	FMC_ADDRESS_WIRE_GPIO_CLK_EN;
	FMC_DATA_WIRE_GPIO_CLK_EN;
	FMC_CONTROL_WIRE_GPIO_CLK_EN;

	//将所有使用到的GPIO引脚初始化为FMC模式
	GPIO_InitStructure.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull=GPIO_PULLUP;
	GPIO_InitStructure.Speed=GPIO_SPEED_HIGH;
	GPIO_InitStructure.Alternate=GPIO_AF12_FMC;
	
	//地址信号线引脚初始化
	GPIO_InitStructure.Pin=FMC_A0_GPIO_PIN;
	HAL_GPIO_Init(FMC_A0_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A1_GPIO_PIN;
	HAL_GPIO_Init(FMC_A1_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A2_GPIO_PIN;
	HAL_GPIO_Init(FMC_A2_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A3_GPIO_PIN;
	HAL_GPIO_Init(FMC_A3_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A4_GPIO_PIN;
	HAL_GPIO_Init(FMC_A4_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A5_GPIO_PIN;
	HAL_GPIO_Init(FMC_A5_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A6_GPIO_PIN;
	HAL_GPIO_Init(FMC_A6_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A7_GPIO_PIN;
	HAL_GPIO_Init(FMC_A7_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A8_GPIO_PIN;
	HAL_GPIO_Init(FMC_A8_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A9_GPIO_PIN;
	HAL_GPIO_Init(FMC_A9_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A10_GPIO_PIN;
	HAL_GPIO_Init(FMC_A10_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_A11_GPIO_PIN;
	HAL_GPIO_Init(FMC_A11_GPIO_PORT,&GPIO_InitStructure);
	
	//数据信号线引脚初始化
	GPIO_InitStructure.Pin=FMC_D0_GPIO_PIN;
	HAL_GPIO_Init(FMC_D0_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D1_GPIO_PIN;
	HAL_GPIO_Init(FMC_D1_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D2_GPIO_PIN;
	HAL_GPIO_Init(FMC_D2_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D3_GPIO_PIN;
	HAL_GPIO_Init(FMC_D3_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D4_GPIO_PIN;
	HAL_GPIO_Init(FMC_D4_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D5_GPIO_PIN;
	HAL_GPIO_Init(FMC_D5_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D6_GPIO_PIN;
	HAL_GPIO_Init(FMC_D6_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D7_GPIO_PIN;
	HAL_GPIO_Init(FMC_D7_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D8_GPIO_PIN;
	HAL_GPIO_Init(FMC_D8_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D9_GPIO_PIN;
	HAL_GPIO_Init(FMC_D9_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D10_GPIO_PIN;
	HAL_GPIO_Init(FMC_D10_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D11_GPIO_PIN;
	HAL_GPIO_Init(FMC_D11_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D12_GPIO_PIN;
	HAL_GPIO_Init(FMC_D12_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D13_GPIO_PIN;
	HAL_GPIO_Init(FMC_D13_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D14_GPIO_PIN;
	HAL_GPIO_Init(FMC_D14_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_D15_GPIO_PIN;
	HAL_GPIO_Init(FMC_D15_GPIO_PORT,&GPIO_InitStructure);
	
	//控制信号线引脚初始化
	GPIO_InitStructure.Pin=FMC_CS_GPIO_PIN;
	HAL_GPIO_Init(FMC_CS_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_BA0_GPIO_PIN;
	HAL_GPIO_Init(FMC_BA0_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_BA1_GPIO_PIN;
	HAL_GPIO_Init(FMC_BA1_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_WE_GPIO_PIN;
	HAL_GPIO_Init(FMC_WE_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_RAS_GPIO_PIN;
	HAL_GPIO_Init(FMC_RAS_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_CAS_GPIO_PIN;
	HAL_GPIO_Init(FMC_CAS_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_CLK_GPIO_PIN;
	HAL_GPIO_Init(FMC_CLK_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_CKE_GPIO_PIN;
	HAL_GPIO_Init(FMC_CKE_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_UDQM_GPIO_PIN;
	HAL_GPIO_Init(FMC_UDQM_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin=FMC_LDQM_GPIO_PIN;
	HAL_GPIO_Init(FMC_LDQM_GPIO_PORT,&GPIO_InitStructure);
}

void SDRAM_Config(void)
{
	FMC_SDRAM_TimingTypeDef SDRAM_Timing;

	SDRAM_Handle.Instance = FMC_SDRAM_DEVICE;
	SDRAM_Handle.Init.SDBank = FMC_SDRAM_BANK2;   //FMC映射存储区域2
	SDRAM_Handle.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;   //列地址选通延时
	SDRAM_Handle.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;   //列地址位数为8
	SDRAM_Handle.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;   //行地址位数为12
	SDRAM_Handle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;   //SDRAM内部Bank数量为4个
	SDRAM_Handle.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;   //数据线宽度为16位
	SDRAM_Handle.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;   //使能突发传输模式
	SDRAM_Handle.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;   //读通道延时
	SDRAM_Handle.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;   //SDRAM时钟信号为HCLK的2分频，为90MHz
	SDRAM_Handle.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;   //禁止写保护

	SDRAM_Timing.ExitSelfRefreshDelay = 7;   //退出自我刷新到行有效命令之间的延迟
	SDRAM_Timing.LoadToActiveDelay = 2;   //加载模式寄存器命令到行有效或刷新命令之间的延迟
	SDRAM_Timing.RCDDelay = 2;   //行有效与列读写命令之间的延迟
	SDRAM_Timing.RowCycleDelay = 7;   //两个刷新命令或两个行有效命令之间的延迟
	SDRAM_Timing.RPDelay = 2;   //预充电与行有效命令之间的延迟
	SDRAM_Timing.SelfRefreshTime = 4;   //行有效与预充电命令之间的延迟
	SDRAM_Timing.WriteRecoveryTime = 2;   //写入命令到预充电命令之间的延迟

	HAL_SDRAM_Init(&SDRAM_Handle, &SDRAM_Timing);

  SDRAM_InitSequence();   //SDRAM初始化时序
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
  __HAL_RCC_FMC_CLK_ENABLE();

  SDRAM_GPIO_Config();
}

/**
 * @brief SDRAM初始化时序
 * 
 */
static void SDRAM_InitSequence(void)
{
  uint32_t modeRegValue = 0;

  FMC_SDRAM_CommandTypeDef Command;

  //配置命令：开启提供给SDRAM的时钟
  Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = modeRegValue;
  HAL_SDRAM_SendCommand(&SDRAM_Handle, &Command, 1000);

  //稳定提供时钟至少100us
  HAL_Delay(1);

  //配置命令：对所有的Bank预充电
  Command.CommandMode = FMC_SDRAM_CMD_PALL;
  HAL_SDRAM_SendCommand(&SDRAM_Handle, &Command, 1000);

  //配置命令：自动刷新(至少2个自动刷新命令)
  Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.AutoRefreshNumber = 8;   //发送8个自动刷新命令
  HAL_SDRAM_SendCommand(&SDRAM_Handle, &Command, 1000);

  //配置命令：设置SDRAM的模式寄存器的值
  modeRegValue = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2 |
                           SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                           SDRAM_MODEREG_CAS_LATENCY_3 |
                           SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                           SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = modeRegValue;
  HAL_SDRAM_SendCommand(&SDRAM_Handle, &Command, 1000);

  //设置FMC的刷新计数器，每个SDCLK周期自动减一，计数到0时自动向SDRAM发送自动刷新命令
  HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handle, 1386);
}




