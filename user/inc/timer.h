#ifndef __TIM_H__
#define __TIM_H__
#include "stm32f4xx.h"
typedef struct
{
	int shiwei;
	int gewei;
}Time_InitTypeDef;
void TIM3_init(u16 arr, u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc,u16 pulse);
void TIM5_CH1_Cap_Init(u32 arr, u16 psc);

#endif

