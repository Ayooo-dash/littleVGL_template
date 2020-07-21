#include "led.h"

void LED_init(void)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	GPIO_InitTypeDef led;
	led.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	led.GPIO_Mode = GPIO_Mode_OUT;
	led.GPIO_OType = GPIO_OType_PP;
	led.GPIO_PuPd = GPIO_PuPd_UP;
	led.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &led);
	GPIO_SetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);
}

void led_ctl(int LEDx, int LED_ONOFF)
{
	switch (LEDx)
	{
	case LED0:
	{
		if (LED_ONOFF == LED_ON)
			GPIO_ResetBits(GPIOF, GPIO_Pin_9);
		else
			GPIO_SetBits(GPIOF, GPIO_Pin_9);
		break;
	}

	case LED1:
	{
		if (LED_ONOFF == LED_ON)
			GPIO_ResetBits(GPIOF, GPIO_Pin_10);
		else
			GPIO_SetBits(GPIOF, GPIO_Pin_10);
		break;
	}

	default:
		break;
	}
}
