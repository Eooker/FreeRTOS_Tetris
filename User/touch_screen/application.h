#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "bsp_lcd.h"

#define DEFAULT_DOWN_SPEED 200

void Game_UI_Init(void);
void Control_Task(void *parameter);
void Down_Task(void *parameter);

#endif
