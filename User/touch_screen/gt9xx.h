#ifndef _GOODIX_GT9XX_H_
#define _GOODIX_GT9XX_H_

#include "bsp_i2c_touch_screen.h"
#include "bsp_lcd.h"
#include "application.h"

//I2C读数据
#define I2C_M_RD   0x0001

//存储I2C通讯的信息
struct i2c_msg
{
  uint8_t addr;    //从设备的I2C地址
  uint16_t flags;   //控制标志
  uint16_t len;   //读写数据的长度
  uint8_t * buf;   //存储读写数据的指针
};

//触摸屏参数
typedef struct
{
  uint16_t max_width;   //触点最大值，高
  uint16_t max_height;   //触点最大值，宽

  uint16_t config_reg_addr;   //配置寄存器地址
} TOUCH_PARAM_TypeDef;

typedef enum
{
  GT9157 = 0,
  GT911 = 1,
  GT5688 = 2,
  GT917S = 3
} TOUCH_IC;
extern TOUCH_IC touchIC;
extern const TOUCH_PARAM_TypeDef touch_param[];

void GTP_TouchProcess(void);
int32_t GTP_Init_Panel(void);
int32_t GTP_Read_Version(void);

extern uint16_t show_len;
extern uint16_t total_len;

//***************************PART1:ON/OFF define*******************************
#define GTP_DEBUG_ON          1
#define GTP_DEBUG_ARRAY_ON    0
#define GTP_DEBUG_FUNC_ON     0

// STEP_2(REQUIRED): Customize your I/O ports & I/O operations
#define GTP_RST_PORT    S5PV210_GPJ3(6)
#define GTP_INT_PORT    S5PV210_GPH1(3)
#define GTP_INT_IRQ     gpio_to_irq(GTP_INT_PORT)
#define GTP_INT_CFG     S3C_GPIO_SFN(0xF)

// STEP_3(optional): Specify your special config info if needed
#define GTP_MAX_HEIGHT   4096
#define GTP_MAX_WIDTH    4096
#define GTP_INT_TRIGGER  1
#define GTP_MAX_TOUCH         5
#define GTP_ESD_CHECK_CIRCLE  2000      // jiffy: ms


//***************************PART3:OTHER define*********************************
#define GTP_DRIVER_VERSION    "V1.8<2013/06/08>"
#define GTP_I2C_NAME          "Goodix-TS"
#define GTP_POLL_TIME         10     // jiffy: ms
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MIN_LENGTH 186
#define GTP_CONFIG_MAX_LENGTH 240
#define FAIL                  0
#define SUCCESS               1
#define SWITCH_OFF            0
#define SWITCH_ON             1

// Registers define
#define GTP_READ_COOR_ADDR    0x814E
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814A
#define GTP_REG_CONFIG_DATA   0x8047
#define GTP_REG_VERSION       0x8140

#define RESOLUTION_LOC        3
#define TRIGGER_LOC           8

#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))
// Log define
#define GTP_INFO(fmt,arg...)           printf("<<-GTP-INFO->> "fmt"\n",##arg)
#define GTP_ERROR(fmt,arg...)          printf("<<-GTP-ERROR->> "fmt"\n",##arg)
#define GTP_DEBUG(fmt,arg...)          do{\
                                         if(GTP_DEBUG_ON)\
                                         printf("<<-GTP-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)
#define GTP_DEBUG_ARRAY(array, num)    do{\
                                         s32 i;\
                                         uint8_t* a = array;\
                                         if(GTP_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-GTP-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)
#define GTP_DEBUG_FUNC()               do{\
                                         if(GTP_DEBUG_FUNC_ON)\
                                         printf("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

#endif /* _GOODIX_GT9XX_H_ */
