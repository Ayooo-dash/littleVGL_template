#include "stm32f4xx.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "timer.h"
#include "usart.h"
#include "lcd.h"
#include "dma.h"
#include "adc.h"
#include "sram.h"
#include "touch.h"
#include "tpad.h"
#include "string.h"
#include "rtc.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "task.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置系统中断优先级分组2
	usart1_init(115200);							//初始化串口波特率为115200
	beep_init();
	LED_init(); //初始化LED
	LCD_Init(); //LCD初始化
	//LCD_Display_Dir(1);
	FSMC_SRAM_Init();
	key_init();
	//key_init_int();		//按键初始化
	TIM3_init(999, 83); //定时器初始化(1ms中断),用于给lvgl提供1ms的心跳节拍
	tp_dev.init();		//触摸屏初始化

	lv_init();			  //lvgl系统初始化
	lv_port_disp_init();  //lvgl显示接口初始化,放在lv_init()的后面
	lv_port_indev_init(); //lvgl输入接口初始化,放在lv_init()的后面

	task_test_start(); //运行例程

	while (1)
	{
		tp_dev.scan(0);
		lv_task_handler();
		key_handler();
	}
}
