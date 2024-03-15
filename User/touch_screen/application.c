#include "application.h"
#include "FreeRTOS.h"
#include "test_key.h"
#include "task.h"
#include "delay.h"
#include "queue.h"
#include <string.h>

RNG_HandleTypeDef RNG_Handle;

extern QueueHandle_t KeyQueue_Handle;

uint32_t g_bottom_block[39] = {0};   //存储底部积累的方块的信息，每一个数据代表一行，共38行，第39行为底部边界，用于比较数据，
                                     //从数据的最高位开始，每一位代表一个方块，表示该位置是否有方块存在，1表示有方块，0表示没有方块

uint16_t Game_score = 0;
uint8_t Game_over_flag = 0;
uint16_t Down_speed = DEFAULT_DOWN_SPEED;

//定义方块形状
typedef enum
{
    b_line = 1,
    b_lineLeft,
    b_lineRight,
    b_lineMiddle,
    b_square,
    b_rightZ,
    b_leftZ,
} block_form_t;

//定义方块基本信息结构体
typedef struct
{
    block_form_t block; //方块形状
    uint16_t angle;     //角度
    uint32_t color;     //颜色
    uint8_t pos_x_min;  //左边界
    uint8_t pos_x_max;  //右边界
    uint8_t high;       //底部高度，即方块底部所在的行数，数字越大高度越低
    uint8_t pos_x[4];   //每个小方块的x坐标，范围：0~18，小方块编号顺序先从左到右，再从上到下
    uint8_t pos_y[4];   //每个小方块的y坐标，范围：0~37
} block_t;

block_t current_block;   //当前控制的方块

static void RNG_Creat_Block(block_t * block);
static void draw_block(uint8_t pos_x, uint8_t pos_y, uint32_t color);
static void erase_block(uint8_t pos_x, uint8_t pos_y);
static void move_left(void);
static void move_right(void);
static void move_down(void);
static void line_rotate(void);
static void rotate_block(void);
static void lineLeft_rotate(void);
static void lineRight_rotate(void);
static void lineMiddle_rotate(void);
static void rightZ_rotate(void);
static void leftZ_rotate(void);
static void scoring(uint8_t NumOfFinish, uint8_t * finish_row, uint8_t bottom_high);
static void Draw_new_block(block_t * block);

/**
 * @brief 初始化随机数生成器
 * 
 * @return uint8_t 初始化成功则返回HAL_OK
 */
uint8_t RNG_Init(void)
{
	uint16_t retry = 0;

	__RNG_CLK_ENABLE();

	RNG_Handle.Instance = RNG;
	HAL_RNG_Init(&RNG_Handle);
	while((__HAL_RNG_GET_FLAG(&RNG_Handle, RNG_FLAG_DRDY) == RESET) && retry < 10000)
	{
		retry++;
		delay_us(10);
	}
	if(retry >= 10000)   //RNG工作不正常
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

/**
* @brief 得到某个范围内的随机数
* @param min,max: 最小,最大值.
* @retval 得到的随机数(rval),满足:min<=rval<=max
*/
int rng_get_random_range(int min, int max)
{
    uint32_t randomnum = 0;
    HAL_RNG_GenerateRandomNumber(&RNG_Handle, &randomnum);
    return randomnum % (max - min + 1) + min;
}

static void Draw_new_block(block_t * block)
{
    LCD_SetTextColor(LCD_COLOR_BLACK);
    draw_block(block->pos_x[0], block->pos_y[0], block->color);
    draw_block(block->pos_x[1], block->pos_y[1], block->color);
    draw_block(block->pos_x[2], block->pos_y[2], block->color);
    draw_block(block->pos_x[3], block->pos_y[3], block->color);
}

/**
 * @brief 随机产生新的方块
 * 
 * @param block 指向当前方块结构体的指针
 */
static void RNG_Creat_Block(block_t * block)
{
    uint8_t RNG_num = (uint8_t)rng_get_random_range(1, 7);   //产生随机数，用于随机生成方块
    switch(RNG_num)
    {
        case 1:
                block->block = b_line;
                block->angle = 0;
                block->color = LCD_COLOR_LIGHTYELLOW;
                block->high = 0;
                block->pos_x_min = 7;
                block->pos_x_max = 10;
                block->pos_x[0] = 7;block->pos_x[1] = 8;block->pos_x[2] = 9;block->pos_x[3] = 10;
                block->pos_y[0] = 0;block->pos_y[1] = 0;block->pos_y[2] = 0;block->pos_y[3] = 0;
                break;
        case 2:
                block->block = b_lineLeft;
                block->angle = 0;
                block->color = LCD_COLOR_DEEPPINK;
                block->high = 1;
                block->pos_x_min = 8;
                block->pos_x_max = 10;
                block->pos_x[0] = 8;block->pos_x[1] = 8;block->pos_x[2] = 9;block->pos_x[3] = 10;
                block->pos_y[0] = 0;block->pos_y[1] = 1;block->pos_y[2] = 1;block->pos_y[3] = 1;
                break;
        case 3:
                block->block = b_lineRight;
                block->angle = 0;
                block->color = LCD_COLOR_LIGHTBLUE;
                block->high = 1;
                block->pos_x_min = 8;
                block->pos_x_max = 10;
                block->pos_x[0] = 10;block->pos_x[1] = 8;block->pos_x[2] = 9;block->pos_x[3] = 10;
                block->pos_y[0] = 0;block->pos_y[1] = 1;block->pos_y[2] = 1;block->pos_y[3] = 1;
                break;
        case 4:
                block->block = b_lineMiddle;
                block->angle = 0;
                block->color = LCD_COLOR_VIOLET;
                block->high = 1;
                block->pos_x_min = 8;
                block->pos_x_max = 10;
                block->pos_x[0] = 9;block->pos_x[1] = 8;block->pos_x[2] = 9;block->pos_x[3] = 10;
                block->pos_y[0] = 0;block->pos_y[1] = 1;block->pos_y[2] = 1;block->pos_y[3] = 1;
                break;
        case 5:
                block->block = b_square;
                block->angle = 0;
                block->color = LCD_COLOR_GOLD;
                block->high = 1;
                block->pos_x_min = 8;
                block->pos_x_max = 9;
                block->pos_x[0] = 8;block->pos_x[1] = 9;block->pos_x[2] = 8;block->pos_x[3] = 9;
                block->pos_y[0] = 0;block->pos_y[1] = 0;block->pos_y[2] = 1;block->pos_y[3] = 1;
                break;
        case 6:
                block->block = b_rightZ;
                block->angle = 0;
                block->color = LCD_COLOR_LIGHTGREEN;
                block->high = 1;
                block->pos_x_min = 8;
                block->pos_x_max = 10;
                block->pos_x[0] = 9;block->pos_x[1] = 10;block->pos_x[2] = 8;block->pos_x[3] = 9;
                block->pos_y[0] = 0;block->pos_y[1] = 0;block->pos_y[2] = 1;block->pos_y[3] = 1;
                break;
        case 7:
                block->block = b_leftZ;
                block->angle = 0;
                block->color = LCD_COLOR_HOTPINK;
                block->high = 1;
                block->pos_x_min = 8;
                block->pos_x_max = 10;
                block->pos_x[0] = 8;block->pos_x[1] = 9;block->pos_x[2] = 9;block->pos_x[3] = 10;
                block->pos_y[0] = 0;block->pos_y[1] = 0;block->pos_y[2] = 1;block->pos_y[3] = 1;
                break;
        default:
                break;
    }
}

/**
 * @brief 游戏界面初始化
 * 
 */
void Game_UI_Init(void)
{
    LCD_SetTransparency(0, 0);
	LCD_SetTransparency(1, 255);
    
    LCD_SelectLayer(1);
    
    LCD_Clear(LCD_COLOR_BLACK);   //背景色为黑色

    LCD_SetTextColor(LCD_COLOR_YELLOW);

    LCD_DrawRect(284, 9, 232, 462, 1);   //游戏外框

    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_SetBackColor(LCD_COLOR_BLACK);

    LCD_FillRect(50, 200, 100, 100, LCD_COLOR_GOLD);   //左移键
    LCD_DrawRect(50, 200, 100, 100, 3);

    LCD_FillRect(650, 200, 100, 100, LCD_COLOR_GOLD);   //右移键
    LCD_DrawRect(650, 200, 100, 100, 3);

    LCD_FillRect(650, 350, 100, 100, LCD_COLOR_LIGHTBLUE);   //旋转键
    LCD_DrawRect(650, 350, 100, 100, 3);

    LCD_FillRect(50, 350, 100, 100, LCD_COLOR_VIOLET);   //快速下降键
    LCD_DrawRect(50, 350, 100, 100, 3);

    LCD_DisplayStringAt(560, 60, "Score: 0");   //游戏得分

    LCD_SetTextColor(LCD_COLOR_BLACK);
		
	RNG_Init();   //初始化随机数生成器

    RNG_Creat_Block(&current_block);   //随机生成方块

    Draw_new_block(&current_block);   //绘制生成的方块
}

/**
 * @brief 绘制单个小方块
 * 
 * @param pos_x 小方块x坐标
 * @param pos_y 小方块y坐标
 * @param color 颜色
 */
static void draw_block(uint8_t pos_x, uint8_t pos_y, uint32_t color)
{
    LCD_SetTextColor(LCD_COLOR_BLACK);
    LCD_FillRect(286 + pos_x * 12, 13 + pos_y * 12, 12, 12, color);   //绘制小方块内部
    LCD_DrawRect(286 + pos_x * 12, 13 + pos_y * 12, 12, 12, 1);   //绘制小方块外框
}

/**
 * @brief 擦除单个小方块
 * 
 * @param pos_x 小方块x坐标
 * @param pos_y 小方块y坐标
 */
static void erase_block(uint8_t pos_x, uint8_t pos_y)
{
    LCD_FillRect(286 + pos_x * 12, 13 + pos_y * 12, 12, 12, LCD_COLOR_BLACK);
}

/**
 * @brief 左移方块
 * 
 */
static void move_left(void)
{
    uint8_t n = 4;

    //确认方块移动后是否会和其它方块重叠
    while(n--)
    {
        if(g_bottom_block[current_block.pos_y[n]] & (0x80000000 >> (current_block.pos_x[n] - 1)))
        {
            return;   //移动后会重叠，不移动直接退出
        }
    }

	n = 0;

    //擦除当前方块并改变方块坐标
    while(n < 4)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
        current_block.pos_x[n]--;
        n++;
    }

    //用新的方块坐标重新绘制方块
	while(n--)
	{
		draw_block(current_block.pos_x[n], current_block.pos_y[n], current_block.color);
	}

    //修改方块的左右边界信息
    current_block.pos_x_min--;
    current_block.pos_x_max--;
}

/**
 * @brief 右移方块
 * 
 */
static void move_right(void)
{
    uint8_t n = 4;
    while(n--)
    {
        if(g_bottom_block[current_block.pos_y[n]] & (0x80000000 >> (current_block.pos_x[n] + 1)))
        {
            return;
        }
    }
	n = 0;
	while(n < 4)
	{
		erase_block(current_block.pos_x[n], current_block.pos_y[n]);
		current_block.pos_x[n]++;
		n++;
	}
    while(n--)
    {
		draw_block(current_block.pos_x[n], current_block.pos_y[n], current_block.color);
    }
    current_block.pos_x_min++;
    current_block.pos_x_max++;
}

/**
 * @brief 旋转lineLeft方块
 * 
 */
static void lineLeft_rotate(void)
{
    uint8_t n = 4;
    block_t new_block;   //定义新的方块结构体，用于保存方块旋转后的信息

    new_block = current_block;

    if( 0 == current_block.angle)   //当前方块角度为0度，旋转至90度
    {
        if(current_block.high > 38)   //垂直方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 90;

        new_block.high++;

        new_block.pos_x[0] += 2;

        new_block.pos_x[1]++;
        new_block.pos_y[1]--;

        new_block.pos_x[3]--;
        new_block.pos_y[3]++;

        new_block.pos_x_min++;
    }
    else if(90 == current_block.angle)   //当前方块角度为90度，旋转至180度
    {
        if(current_block.pos_x_min < 1)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 180;

        new_block.pos_y[0] += 2;

        new_block.pos_x[1]++;
        new_block.pos_y[1]++;

        new_block.pos_x[3]--;
        new_block.pos_y[3]--;

        new_block.pos_x_min--;
    }
    else if(180 == current_block.angle)   //当前方块角度为180度，旋转至270度
    {
        //180度向270旋转时不会改变左边界和底部高度，右边界只会减小，所以不用判断旋转空间是否足够

        new_block.angle = 270;

        new_block.pos_x[0] -= 2;

        new_block.pos_x[1]--;
        new_block.pos_y[1]++;

        new_block.pos_x[3]++;
        new_block.pos_y[3]--;

        new_block.pos_x_max--;
    }
    else if(270 == current_block.angle)   //当前方块角度为270度，旋转至0度
    {
        if(current_block.pos_x_max > 17)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 0;

        new_block.high--;

        new_block.pos_y[0] -= 2;

        new_block.pos_x[1]--;
        new_block.pos_y[1]--;

        new_block.pos_x[3]++;
        new_block.pos_y[3]++;

        new_block.pos_x_max++;
    }

    //判断旋转后的方块是否会和其它方块重叠
    while(n--)
    {
        if(g_bottom_block[new_block.pos_y[n]] & (0x80000000 >> new_block.pos_x[n]))
        {
            return;   //重叠了，丢弃新方块，直接退出
        }
    }

    n = 4;

    //擦除当前方块
    while(n--)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
    }

    n = 4;

    //绘制新方块
    while(n--)
    {
        draw_block(new_block.pos_x[n], new_block.pos_y[n], new_block.color);
    }

    //将新方块的信息拷贝到当前方块
    current_block = new_block;
}

/**
 * @brief 旋转lineRight方块
 * 
 */
static void lineRight_rotate(void)
{
    uint8_t n = 4;
    block_t new_block;

    new_block = current_block;

    if( 0 == current_block.angle)
    {
        if(current_block.high > 38)   //垂直方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 90;

        new_block.high++;

        new_block.pos_y[0] += 2;

        new_block.pos_x[1]++;
        new_block.pos_y[1]--;

        new_block.pos_x[3]--;
        new_block.pos_y[3]++;

        new_block.pos_x_min++;
    }
    else if(90 == current_block.angle)
    {
        if(current_block.pos_x_min < 1)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 180;

        new_block.pos_x[0] -= 2;

        new_block.pos_x[1]++;
        new_block.pos_y[1]++;

        new_block.pos_x[3]--;
        new_block.pos_y[3]--;

        new_block.pos_x_min--;
    }
    else if(180 == current_block.angle)
    {
        new_block.angle = 270;

        new_block.pos_y[0] -= 2;

        new_block.pos_x[1]--;
        new_block.pos_y[1]++;

        new_block.pos_x[3]++;
        new_block.pos_y[3]--;

        new_block.pos_x_max--;
    }
    else if(270 == current_block.angle)
    {
        if(current_block.pos_x_max > 17)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 0;

        new_block.high--;

        new_block.pos_x[0] += 2;

        new_block.pos_x[1]--;
        new_block.pos_y[1]--;

        new_block.pos_x[3]++;
        new_block.pos_y[3]++;

        new_block.pos_x_max++;
    }

    while(n--)
    {
        if(g_bottom_block[new_block.pos_y[n]] & (0x80000000 >> new_block.pos_x[n]))
        {
            return;
        }
    }

    n = 4;
    while(n--)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
    }

    n = 4;
    while(n--)
    {
        draw_block(new_block.pos_x[n], new_block.pos_y[n], new_block.color);
    }

    current_block = new_block;
}

/**
 * @brief 旋转lineMiddle方块
 * 
 */
static void lineMiddle_rotate(void)
{
    uint8_t n = 4;
    block_t new_block;

    new_block = current_block;

    if( 0 == current_block.angle)
    {
        if(current_block.high > 38)   //垂直方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 90;

        new_block.high++;

        new_block.pos_x[0]++;
        new_block.pos_y[0]++;

        new_block.pos_x[1]++;
        new_block.pos_y[1]--;

        new_block.pos_x[3]--;
        new_block.pos_y[3]++;

        new_block.pos_x_min++;
    }
    else if(90 == current_block.angle)
    {
        if(current_block.pos_x_min < 1)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 180;

        new_block.pos_x[0]--;
        new_block.pos_y[0]++;

        new_block.pos_x[1]++;
        new_block.pos_y[1]++;

        new_block.pos_x[3]--;
        new_block.pos_y[3]--;

        new_block.pos_x_min--;
    }
    else if(180 == current_block.angle)
    {
        new_block.angle = 270;

        new_block.pos_x[0]--;
        new_block.pos_y[0]--;

        new_block.pos_x[1]--;
        new_block.pos_y[1]++;

        new_block.pos_x[3]++;
        new_block.pos_y[3]--;

        new_block.pos_x_max--;
    }
    else if(270 == current_block.angle)
    {
        if(current_block.pos_x_max > 17)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 0;

        new_block.high--;

        new_block.pos_x[0]++;
        new_block.pos_y[0]--;

        new_block.pos_x[1]--;
        new_block.pos_y[1]--;

        new_block.pos_x[3]++;
        new_block.pos_y[3]++;

        new_block.pos_x_max++;
    }

    while(n--)
    {
        if(g_bottom_block[new_block.pos_y[n]] & (0x80000000 >> new_block.pos_x[n]))
        {
            return;
        }
    }

    n = 4;
    while(n--)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
    }

    n = 4;
    while(n--)
    {
        draw_block(new_block.pos_x[n], new_block.pos_y[n], new_block.color);
    }

    current_block = new_block;
}

/**
 * @brief 旋转line方块
 * 
 */
static void line_rotate(void)
{
    uint8_t n = 4;
    block_t new_block;

    new_block = current_block;

    //line方块的180度和270度与0度和90度是一样的，所以只旋转两个角度

    if( 0 == current_block.angle)  //目前是水平直线
    {
        if(current_block.high > 37)   //垂直方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 90;
        new_block.pos_x_min++;
        new_block.pos_x_max -= 2;

        if(current_block.high > 0)   //当前高度足够以第二个块进行旋转
        {
            new_block.pos_x[0]++;
            new_block.pos_y[0]--;

            new_block.pos_x[2]--;
            new_block.pos_y[2]++;

            new_block.pos_x[3] -= 2;
            new_block.pos_y[3] += 2;

            new_block.high += 2;
        }
        else   //当前高度不足够以第二个块进行旋转
        {
            new_block.pos_x[0]++;

            new_block.pos_y[1]++;

            new_block.pos_x[2]--;
            new_block.pos_y[2] += 2;

            new_block.pos_x[3] -= 2;
            new_block.pos_y[3] += 3;

            new_block.high += 3;
        }
    }
    else  //目前是垂直直线
    {
        if(current_block.pos_x_min < 1 || current_block.pos_x_max > 16)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 0;
        new_block.pos_x_min--;
        new_block.pos_x_max += 2;

        new_block.pos_x[0]--;
        new_block.pos_y[0]++;

        new_block.pos_x[2]++;
        new_block.pos_y[2]--;
        
        new_block.pos_x[3] += 2;
        new_block.pos_y[3] -= 2;

        new_block.high -= 2;
    }

    while(n--)
    {
        if(g_bottom_block[new_block.pos_y[n]] & (0x80000000 >> new_block.pos_x[n]))
        {
            return;
        }
    }

    n = 4;
    while(n--)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
    }

    n = 4;
    while(n--)
    {
        draw_block(new_block.pos_x[n], new_block.pos_y[n], new_block.color);
    }

    current_block = new_block;
}

/**
 * @brief 旋转rightZ方块
 * 
 */
static void rightZ_rotate(void)
{
    uint8_t n = 4;
    block_t new_block;

    new_block = current_block;

    //rightZ方块的180度和270度与0度和90度是一样的，所以只旋转两个角度

    if( 0 == current_block.angle)
    {
        if(current_block.high > 38)   //垂直方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 90;

        new_block.high++;

        new_block.pos_x[0]++;
        new_block.pos_y[0]++;

        new_block.pos_y[1] += 2;

        new_block.pos_x[2]++;
        new_block.pos_y[2]--;

        new_block.pos_x_min++;
    }
    else if(90 == current_block.angle)
    {
        if(current_block.pos_x_min < 1)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 0;

        new_block.high--;

        new_block.pos_x[0]--;
        new_block.pos_y[0]--;

        new_block.pos_y[1] -= 2;

        new_block.pos_x[2]--;
        new_block.pos_y[2]++;

        new_block.pos_x_min--;
    }

    while(n--)
    {
        if(g_bottom_block[new_block.pos_y[n]] & (0x80000000 >> new_block.pos_x[n]))
        {
            return;
        }
    }

    n = 4;
    while(n--)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
    }

    n = 4;
    while(n--)
    {
        draw_block(new_block.pos_x[n], new_block.pos_y[n], new_block.color);
    }

    current_block = new_block;
}

/**
 * @brief 旋转leftZ方块
 * 
 */
static void leftZ_rotate(void)
{
    uint8_t n = 4;
    block_t new_block;

    new_block = current_block;

    //leftZ方块的180度和270度与0度和90度是一样的，所以只旋转两个角度

    if( 0 == current_block.angle)
    {
        if(current_block.high > 38)   //垂直方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 90;

        new_block.high++;

        new_block.pos_x[0] += 2;

        new_block.pos_x[1]++;
        new_block.pos_y[1]++;

        new_block.pos_x[3]--;
        new_block.pos_y[3]++;

        new_block.pos_x_min++;
    }
    else if(90 == current_block.angle)
    {
        if(current_block.pos_x_min < 1)   //水平方向空间不够，无法旋转
        {
            return;
        }

        new_block.angle = 0;

        new_block.high--;

        new_block.pos_x[0] -= 2;

        new_block.pos_x[1]--;
        new_block.pos_y[1]--;

        new_block.pos_x[3]++;
        new_block.pos_y[3]--;

        new_block.pos_x_min--;
    }

    while(n--)
    {
        if(g_bottom_block[new_block.pos_y[n]] & (0x80000000 >> new_block.pos_x[n]))
        {
            return;
        }
    }

    n = 4;
    while(n--)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
    }

    n = 4;
    while(n--)
    {
        draw_block(new_block.pos_x[n], new_block.pos_y[n], new_block.color);
    }

    current_block = new_block;
}

/**
 * @brief 旋转当前方块
 * 
 */
static void rotate_block(void)
{
    switch(current_block.block)
    {
        case b_line:line_rotate();break;
        case b_lineLeft:lineLeft_rotate();break;
        case b_lineRight:lineRight_rotate();break;
        case b_lineMiddle:lineMiddle_rotate();break;
        case b_square:break;   //square方块旋转后没变化，所以不用旋转
        case b_rightZ:rightZ_rotate();break;
        case b_leftZ:leftZ_rotate();break;
    }
}

/**
 * @brief 下移方块
 * 
 */
static void move_down(void)
{
    uint8_t n = 4;

    //确认方块移动后是否会和其它方块重叠
    while(n--)
    {
        if(g_bottom_block[current_block.pos_y[n] + 1] & (0x80000000 >> (current_block.pos_x[n])))
        {
            return;   //移动后会重叠，不移动直接退出
        }
    }

    n = 4;

    //擦除当前方块并下移y坐标
    while(n--)
    {
        erase_block(current_block.pos_x[n], current_block.pos_y[n]);
        current_block.pos_y[n]++;
    }

	n = 4;

    //根据新的坐标重新绘制方块
	while(n--)
	{
		draw_block(current_block.pos_x[n], current_block.pos_y[n], current_block.color);
	}

    //更改方块底部高度
    current_block.high++;
}

/**
 * @brief 消除完成的方块并计算得分
 * 
 * @param NumOfFinish 需要消除的行数
 * @param finish_row 存放要消除的行的位置
 * @param bottom_high 底部积累的方块的最大高度
 */
static void scoring(uint8_t NumOfFinish, uint8_t * finish_row, uint8_t bottom_high)
{
    uint32_t remove_addr[NumOfFinish];
    uint32_t buffer_addr[NumOfFinish];
    uint32_t destinAddr;
    uint32_t sourceAddr;
    uint8_t flicker_num = 2;
    uint8_t Score_printf[20];
    uint8_t i, j, k;

    //将要消除的行先缓存到背景层
    for(i = 0; i < NumOfFinish; i++)
    {
        //获取源地址和缓存地址
        LCD_SelectLayer(0);
        buffer_addr[i] = LCD_GetStartAddress(285, 13 + finish_row[i] * 12);
        LCD_SelectLayer(1);
        remove_addr[i] = LCD_GetStartAddress(285, 13 + finish_row[i] * 12);

        //每一行方块的数据在内存上是不连续的，只能按像素行逐行缓存
        for(j = 0; j < 12; j++)
        {
            //j * LCD_GetXSize() * 4 表示每个像素行的偏移地址
           memcpy((void *)(buffer_addr[i] + j * LCD_GetXSize() * 4), (void *)(remove_addr[i] + j * LCD_GetXSize() * 4), 230 * 4);
        }
    }

    LCD_SelectLayer(1);

    //闪烁提示要消除的行，闪烁两次
    for(; flicker_num > 0; flicker_num--)
    {
        //先擦除要消除的行
        for(i = 0; i < NumOfFinish; i++)
        {
            LCD_FillRect(285, 13 + finish_row[i] * 12, 230, 12, LCD_COLOR_BLACK);
        }

		vTaskDelay(150);

        //再从背景层把消除行拷贝回来
        for(i = 0; i < NumOfFinish; i++)
        {
            for(j = 0; j < 12; j++)
            {
               memcpy((void *)(remove_addr[i] + j * LCD_GetXSize() * 4), (void *)(buffer_addr[i] + j * LCD_GetXSize() * 4), 230 * 4);
            }
        }

		vTaskDelay(150);
    }

    //闪烁结束后不需要擦除消除行，直接用消除行上面的画面覆盖即可
    for(i = 0; i < NumOfFinish; i++)
    {
        //从消除行开始逐行往下移动覆盖
        //注：每次往下滚动一行后，还没消除的消除行也会往下移动一行，所以j要加上i(下降时行坐标增加)，同理最高层也在随着滚动下降
        for(j = finish_row[i] + i; j >= bottom_high + i; j--)
        {
            destinAddr = LCD_GetStartAddress(285, 13 + j * 12);   //被覆盖行起始地址
            sourceAddr = LCD_GetStartAddress(285, 13 + (j - 1) * 12);   //覆盖行起始地址

            //逐像素行进行拷贝覆盖
            for(k = 0; k < 12; k++)
            {
                memcpy((void *)(destinAddr + k * LCD_GetXSize() * 4), (void *)(sourceAddr + k * LCD_GetXSize() * 4), 230 * 4);
            }

            //方块数据库也同时进行覆盖
            g_bottom_block[j] = g_bottom_block[j - 1];
        }
    }

    //计数得分
    Game_score += NumOfFinish;
    sprintf((char *)Score_printf, "Score: %d", Game_score);   //生成得分字符串
    LCD_FillRect(560, 60, 200, 25, LCD_COLOR_BLACK);   //擦除当前得分
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringAt(560, 60, Score_printf);   //重新绘制得分
    LCD_SetTextColor(LCD_COLOR_BLACK); 
}

/**
 * @brief 方块下落任务
 * 
 * @param parameter 
 */
void Down_Task(void *parameter)
{
    int8_t n = 4;
    uint8_t score_text[20];
    uint8_t stop_flag = 0;
    uint8_t finish_row[40];
    uint8_t NumOfFinish = 0;
    uint8_t compare_high = 38;   //比较行，用于与当前方块进行比较，确定方块是否可以继续下落
    g_bottom_block[38] = 0xFFFFFFFF;   //初始化第39行，作为底部边界，用于比较数据

    memset(finish_row, 0xFF, sizeof(finish_row));

    while(1)
    {
        if(!Game_over_flag)
        {
            if((compare_high - current_block.high) > 1)   //当前方块的高度可以继续下落
            {
                move_down();
            }
            else   //当前方块的高度碰到了比较行，需要进行判断
            {
                n = 4;

                //判断方块中的每一个小方块位置，继续下移时是否会和其它方块重叠
                while(n--)
                {
                    if((0x80000000 >> current_block.pos_x[n]) & g_bottom_block[compare_high - (current_block.high - current_block.pos_y[n])])
                    {
                        stop_flag = 1;   //会重叠说明当前方块不可以继续下落，置位停止标志
                        break;   //有一个小方块重叠即停止，其它小方块不用判断
                    }
                }

                //当前方块不能继续下落，处理当前方块
                if(stop_flag)
                {
                    stop_flag = 0;   //清除停止标志

                    //恢复方块的下落速度
                    Down_speed = DEFAULT_DOWN_SPEED;

                    n = 4;

                    //将当前方块的每个小方块位置填入方块数据库
                    while(n--)
                    {
                        g_bottom_block[compare_high - 1 - (current_block.high - current_block.pos_y[n])] |= (0x80000000 >> current_block.pos_x[n]);
                    }

                    n = 39;

                    //判读是否有完成行，并生成新的比较行
                    while(n--)
                    {
                        //如果该行数据等于0xFFFFE000，即表示该行的每个位置都有方块，要消除该行
                        if(g_bottom_block[n] == 0xFFFFE000)
                        {
                            finish_row[NumOfFinish] = n;   //记录下消除行的行坐标
                            NumOfFinish++;   //记录一共有多少行消除行
                        }

                        //如果该行没有方块，说明它的下一行就是底部方块的最高行，将其作为比较行
                        if(g_bottom_block[n] == 0)
                        {
                            compare_high = n + 1;
                            break;   //最高行的上面全是空行，不需要再判断了，直接退出循环
                        }
                    }

                    //判断底部积累的方块是否触顶，触顶则游戏结束
                    if(-1 == n && 0 == NumOfFinish)   //积累的方块已触顶，且没有能够消除的行，游戏结束
                    {
                        Game_over_flag = 1;
                        sprintf((char *)score_text, "%d", Game_score);   //生成得分字符串

                        //绘制游戏结束弹窗
											  LCD_SetTextColor(LCD_COLOR_RED);
                        LCD_FillRect(210, 160, 380, 155, LCD_COLOR_LIGHTYELLOW);
                        LCD_DrawRect(210, 160, 380, 155, 2);

                        //输出游戏评分和提示信息
											  LCD_SetTextColor(LCD_COLOR_RED);
                        LCD_DisplayStringAt(323, 170, "GAME OVER");
                        LCD_DisplayStringAt(307, 195, "Your Score:");
                        LCD_DisplayStringAt((800 - strlen((const char *)score_text) * 17) / 2, 220, score_text);

                        //绘制重新开始按钮
											  LCD_SetTextColor(LCD_COLOR_RED);
                        LCD_FillRect(306, 255, 188, 50, LCD_COLOR_GOLD);
                        LCD_DrawRect(306, 255, 188, 50, 3);
                        LCD_SetTextColor(LCD_COLOR_BLACK);
                        LCD_DisplayStringAt(340, 268, "RESTART");

                        //重新初始化游戏数据，为下次游戏做准备
                        Game_score = 0;
                        compare_high = 38;
                        memset(g_bottom_block, 0, sizeof(g_bottom_block));
                        g_bottom_block[38] = 0xFFFFFFFF;
                    }
                    else   //游戏还没有结束，继续运行
                    {
                        //处理消除行
                        if(NumOfFinish != 0)
                        {
                            //处理消除行并计算得分
                            scoring(NumOfFinish, finish_row, compare_high);

                            //处理掉消除行后最高行也往下移动相应行
                            compare_high += NumOfFinish;

                            //处理完后清除消除行数据
                            memset(finish_row, 0xFF, sizeof(finish_row));
                            NumOfFinish = 0;
                        }

                        //生成新的方块
                        RNG_Creat_Block(&current_block);
                        Draw_new_block(&current_block);
                    }
                }
                else   //当前方块还可以继续下落
                {
                    move_down();
                    compare_high++;   //将下一行作为比较行
                }
            }
        }
        vTaskDelay(Down_speed);
    }
}

/**
 * @brief 方块控制任务
 * 
 * @param parameter 
 */
void Control_Task(void *parameter)
{
    BaseType_t xReturn = pdTRUE;
    uint8_t queueData;
    while(1)
    {
        //获取消息队列发过来的信息，没有信息则一直等待
        xReturn = xQueueReceive(KeyQueue_Handle, &queueData, portMAX_DELAY);

        //根据发过来的信息完成方块控制
        if(pdTRUE == xReturn)
        {
            taskENTER_CRITICAL();   //为了避免在控制方块时被打断，需要进入临界保护

            if(5 == queueData)
            {
                Game_UI_Init();
                Game_over_flag = 0;
            }

            if(1 == queueData && current_block.pos_x_min > 0)   //收到左移指令且有足够空间左移
            {
                move_left();
            }
            else if(2 == queueData && current_block.pos_x_max < 18)   //收到右移指令且有足够空间右移
            {
                move_right(); 
            }
            else if(3 == queueData)   //旋转方块
            {
                rotate_block();
            }
            else if(4 == queueData)   //方块快速下落
            {
                Down_speed = 30;
            }

            taskEXIT_CRITICAL();
        }
        else
        {
            LCD_printf("Receive queue Error: %d \n", xReturn);
        }
    }
}
