#ifndef __ADC_H__
#define __ADC_H__
#include "stm32f4xx.h"

void ADC_Setting_Init(void);
u16 ADC_GetValue(u8 ADC_Channel_x);
u16 ADC_GetAverage(u8 ADC_Channel_x, u8 times);

#endif
