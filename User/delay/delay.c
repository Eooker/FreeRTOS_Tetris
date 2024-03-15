#include "delay.h"

void delay_us(uint32_t nus)
{
  uint32_t ticks, t_old, t_now, t_count;
  uint32_t reload = SysTick->LOAD;
  ticks = nus * 180;
  t_old = SysTick->VAL;
  while(1)
  {
    t_now = SysTick->VAL;
    if(t_now != t_old)   //注意systick是向下计数的
    {
      if(t_now < t_old)
      {
        t_count += (t_old - t_now);
      }
      else
      {
        t_count += t_old + (reload - t_now);
      }
      t_old = t_now;
      if(t_count >= ticks)
      {
        break;
      }
    }
  }
}
