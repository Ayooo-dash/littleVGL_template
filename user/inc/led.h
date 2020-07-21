#ifndef __LED_H__
#define __LED_H__
#include "stm32f4xx.h"
#include "bitband.h"
#define LED_ON 0  //低电平点亮LED--PF9~10
#define LED_OFF 1 //高电平点亮LED--PF9~10
#define LED0 1
#define LED1 2
#define DS0 PFout(9)  //PF9位带操作
#define DS1 PFout(10) //PF10位带操作

void LED_init(void);
void led_ctl(int LEDx, int LED_ONOFF);

#endif
