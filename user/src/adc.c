#include "adc.h"
#include "delay.h"

/*ADC1初始化配置*/
void ADC_Setting_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /*GPIO口，ADC1通道5初始化*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PA5通道5
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);  //ADC1复位
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); //ADC1复位结束
    //ADC_DeInit();

    /*ADC配置*/
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;      //不使能DMA
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                     //独立模式
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                  //4分频，84/4=21Mhz，ADC时钟最好不要超过36Mhz
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //两个采样阶段之间延时5个时钟
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                         //关闭连续转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      //右对齐
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //禁止触发检测，使用软件触发
    ADC_InitStructure.ADC_NbrOfConversion = 1;                                  //1个转换在规则序列中，即只有转换规则序列1
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                      //12位模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                               //非扫描模式
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);
}

/*取出转换值*/
u16 ADC_GetValue(u8 ADC_Channel_x)
{
    ADC_RegularChannelConfig(ADC1, ADC_Channel_x, 1, ADC_SampleTime_480Cycles); //ADC1通道，采用480个周期
    ADC_SoftwareStartConv(ADC1);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;
    return ADC_GetConversionValue(ADC1);
}

/*取平均值*/
u16 ADC_GetAverage(u8 ADC_Channel_x, u8 times)
{
    u32 Sum = 0;
    for (u8 t = 0; t < times; t++)
    {
        Sum += ADC_GetValue(ADC_Channel_x);
        delay_ms(5);
    }
    return Sum / times;
}
