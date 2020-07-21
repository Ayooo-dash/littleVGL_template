#include "timer.h"
#include "led.h"
#include "lcd.h"
#include "lvgl.h"

/*定时器TIM3初始化*/
void TIM3_init(u16 arr, u16 psc)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef t;
	t.TIM_Prescaler = psc; //设置定时器预分频值
	t.TIM_Period = arr;	   //设置自动重装载值
	t.TIM_CounterMode = TIM_CounterMode_Up;
	t.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &t);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

	/*定时器中断配置*/
	NVIC_InitTypeDef tim;
	tim.NVIC_IRQChannel = TIM3_IRQn;
	tim.NVIC_IRQChannelPreemptionPriority = 1;
	tim.NVIC_IRQChannelSubPriority = 3;
	tim.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tim);
}

/*初始化Tim3的PWM波输出--PA6*/
void TIM3_PWM_Init(u16 arr, u16 psc, u16 pulse)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef pa;
	pa.GPIO_Pin = GPIO_Pin_6;
	pa.GPIO_Mode = GPIO_Mode_AF; //PA6复用功能输出
	pa.GPIO_OType = GPIO_OType_PP;
	pa.GPIO_PuPd = GPIO_PuPd_NOPULL;
	pa.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &pa);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef t;
	t.TIM_Prescaler = psc;					//设置定时器预分频值
	t.TIM_Period = arr;						//设置自动重装载值
	t.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	t.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &t);
	TIM_ARRPreloadConfig(TIM3, ENABLE); //定时器重装载使能

	TIM_OCInitTypeDef b;
	b.TIM_OCMode = TIM_OCMode_PWM1;
	b.TIM_OCPolarity = TIM_OCPolarity_High; //设置高电平为有效电平
	b.TIM_Pulse = pulse;					//设置比较值，决定占空比
	b.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM3, &b);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); //通道1重装载使能

	TIM_Cmd(TIM3, ENABLE);
}
u32 TIM5_arr; //TIM5重装载值

/*定时器TIM5输入捕获初始化*/
void TIM5_CH1_Cap_Init(u32 arr, u16 psc) //PA0引脚
{
	TIM5_arr = arr;										  //重装载值
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //使能TIM5时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟

	/*GPIO初始化配置*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);

	/*时基单元配置*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	/*TIM5输入捕获参数初始化*/
	TIM_ICInitTypeDef TIM5_ICInitStructure;
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1;				 //CC1S=01,选择输入端，IC1映射到TI1上
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 //上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM5_ICInitStructure.TIM_ICFilter = 0x00; //IC1F=0000，配置输入滤波器为不滤波
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE); //允许更新中断，允许CC1TE捕获中断
	TIM_Cmd(TIM5, ENABLE);									//使能定时器TIM5

	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//u8 y = 0;
/*定时器TIM3中断服务函数*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update))
	{
		lv_tick_inc(1); //lvgl 的 1ms 心跳
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

//u8 TIM5CH1_CAPTURE_FLAG1 = 0;	//上升沿捕获标志
//u8 TIM5CH1_CAPTURE_FLAG2 = 0;	//下降沿捕获标志或捕获完成标志
//u16 TIM5CH1_CAPTURE_COUT = 0;	//定时器溢出次数
//u16 TIM5CH1_CAPTURE_VALUE = 0;	//高电平时长捕获值
/*定时器TIM5中断服务函数*/
/*void TIM5_IRQHandler(void)
{
	if(TIM5CH1_CAPTURE_FLAG2 == 0)	//捕获未成功
	{
		if(TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)	//定时器溢出触发中断
		{
			if(TIM5CH1_CAPTURE_FLAG1)	//捕获到上升沿
			{
				if(TIM5CH1_CAPTURE_COUT >= 0xFFFF)	//高电平时间过长
				{
					TIM5CH1_CAPTURE_FLAG2 = 1;	//捕获失败，结束捕获
					TIM5CH1_CAPTURE_VALUE = 0xFFFF;
					TIM_Cmd(TIM5, DISABLE);	//关闭定时器TIM5
				}
				else
					TIM5CH1_CAPTURE_COUT++;
			}
		}
		if(TIM_GetITStatus(TIM5, TIM_IT_CC1) == SET)	//捕获事件触发中断
		{
			if(TIM5CH1_CAPTURE_FLAG1 == 0)	//未开始，第一次捕获上升沿
			{
				TIM5CH1_CAPTURE_FLAG1 = 0;		//清空
				TIM5CH1_CAPTURE_FLAG2 = 0;
				TIM5CH1_CAPTURE_VALUE = 0;
				TIM5CH1_CAPTURE_COUT = 0;
				TIM_Cmd(TIM5, DISABLE);		//关闭定时器TIM5
				TIM_SetCounter(TIM5, 0);	//设置计数器为0
				TIM5CH1_CAPTURE_FLAG1 = 1;	//标记已经捕获的上升沿
				TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);	//设置为下降沿捕获
				TIM_Cmd(TIM5, ENABLE);	//使能定时器TIM5
			}
			else
			{
				TIM5CH1_CAPTURE_FLAG2 = 1;	//标记成功捕获
				TIM5CH1_CAPTURE_VALUE = TIM_GetCapture1(TIM5)+TIM5CH1_CAPTURE_COUT*TIM5_arr;
				TIM_Cmd(TIM5, DISABLE);	//关闭定时器TIM5
			}
		}
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update | TIM_IT_CC1);	//清除中断标记位
}*/

u8 TIM5CH1_CAPTURE_STA = 0;	  //输入捕获状态，bit5~0捕获计数器溢出次数，bit6捕获到上升沿标志，bit7捕获完成标志
u32 TIM5CH1_CAPTURE_VAL;	  //输入捕获值(TIM2/TIM5是32位)
u32 TIM5CH1_CAPTURE_TIME = 0; //捕获的高电平总时长
/*定时器5中断服务程序*/
void TIM5_IRQHandler(void)
{
	if ((TIM5CH1_CAPTURE_STA & 0X80) == 0) //还未成功捕获
	{
		if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //溢出
		{
			if (TIM5CH1_CAPTURE_STA & 0X40) //已经捕获到高电平了
			{
				if ((TIM5CH1_CAPTURE_STA & 0X3F) == 0X3F) //高电平太长了
				{
					TIM5CH1_CAPTURE_STA |= 0X80; //标记成功捕获了一次
					TIM5CH1_CAPTURE_VAL = 0XFFFFFFFF;
				}
				else
					TIM5CH1_CAPTURE_STA++;
			}
		}
		if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) //捕获1发生捕获事件
		{
			if (TIM5CH1_CAPTURE_STA & 0X40) //捕获到一个下降沿
			{
				TIM5CH1_CAPTURE_STA |= 0X80;						//标记成功捕获到一次高电平脉宽
				TIM5CH1_CAPTURE_VAL = TIM_GetCapture1(TIM5);		//获取当前的捕获值.
																	//TIM5CH1_CAPTURE_TIME=TIM5CH1_CAPTURE_STA&0X3F;	//取出溢出次数bit5~0，
																	//TIM5CH1_CAPTURE_TIME*=0XFFFFFFFF;	//溢出时间总和，
																	//TIM5CH1_CAPTURE_TIME+=TIM5CH1_CAPTURE_VAL;	//得到总的高电平时间。
				TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
																	//TIM_Cmd(TIM5,DISABLE);
			}
			else //还未开始,第一次捕获上升沿
			{
				TIM5CH1_CAPTURE_STA = 0; //清空
				TIM5CH1_CAPTURE_VAL = 0;
				TIM5CH1_CAPTURE_TIME = 0;
				TIM5CH1_CAPTURE_STA |= 0X40; //标记捕获到了上升沿
				TIM_Cmd(TIM5, DISABLE);		 //关闭定时器5
				TIM_SetCounter(TIM5, 0);
				TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling); //CC1P=1 设置为下降沿捕获
				TIM_Cmd(TIM5, ENABLE);								 //使能定时器5
			}
		}
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update); //清除中断标志位
}
