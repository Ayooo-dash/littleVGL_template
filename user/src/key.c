#include "key.h"
#include "delay.h"
#include "timer.h"
#include "lcd.h"
#include "touch.h"

/*按键初始化--KEY0~2--PE2~4设置为输入模式*/
void key_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);*/
}

/*按键检测函数，按键按下返回KEY_DOWN,反之返回KEY_UP，按键无效返回KEY_ERR*/
key_status_typedef key_check(int KEYx)
{
	key_status_typedef status;
	switch (KEYx)
	{
	case KEY0:
	{
		if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
		{
			delay_ms(10);
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
			{
				while (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)))
					;
				delay_ms(10);
				while (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)))
					;
				status = KEY_DOWN;
			}
			else
				status = KEY_UP;
		}
		else
			status = KEY_UP;
		break;
	}

	case KEY1:
	{
		if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
		{
			delay_ms(10);
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
			{
				while (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)))
					;
				delay_ms(10);
				while (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)))
					;
				status = KEY_DOWN;
			}
			else
				status = KEY_UP;
		}
		else
			status = KEY_UP;
		break;
	}

	case KEY2:
	{
		if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0)
		{
			delay_ms(10);
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0)
			{
				while (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)))
					;
				delay_ms(10);
				while (!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)))
					;
				status = KEY_DOWN;
			}
			else
				status = KEY_UP;
		}
		else
			status = KEY_UP;
		break;
	}

		/*case WK_UP:
		{
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==1)
			{
				delay_ms(10);
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==1)
				{
					while(!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)));
					delay_ms(10);
					while(!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)));
					status = KEY_DOWN;
				}
				else
					status = KEY_UP;
			}
			else
				status = KEY_UP;
			break;
		}*/

	default:
	{
		status = KEY_ERR;
		break;
	}
	}
	return status;
}

/*以外部中断方式初始化按键KEY0~2--PE2~4*/
void key_init_int(void)
{
	/*初始化KEY的GPIO口，设置为输入模式*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef key;
	key.GPIO_Mode = GPIO_Mode_IN;
	key.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	key.GPIO_PuPd = GPIO_PuPd_UP;
	key.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &key);
	key.GPIO_Pin = GPIO_Pin_0;
	key.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &key);

	/*SYSCFG配置*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);

	/*EXTI配置*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_EXTIT, ENABLE);
	EXTI_InitTypeDef f;
	f.EXTI_Line = EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
	f.EXTI_LineCmd = ENABLE;
	f.EXTI_Mode = EXTI_Mode_Interrupt;
	f.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&f);
	f.EXTI_Line = EXTI_Line0;
	f.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&f);

	/*NVIC配置*/
	NVIC_InitTypeDef g;
	g.NVIC_IRQChannel = EXTI2_IRQn;
	g.NVIC_IRQChannelCmd = ENABLE;
	g.NVIC_IRQChannelPreemptionPriority = 2;
	g.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&g);
	g.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&g);
	g.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&g);
	g.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Init(&g);
}

/*外部中断0服务子程序--KEY_UP*/
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0))
	{
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

/*外部中断2服务子程序--KEY2*/
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2))
	{
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}
//u8 dir_switch = 0;
/*外部中断3服务子程序--KEY1*/
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3))
	{
		//dir_switch = !dir_switch;
		//LCD_Display_Dir(dir_switch);
		//LCD_Clear(BLACK);
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

extern u8 key0_up;
/*外部中断4服务子程序--KEY0*/
void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4))
	{
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}
