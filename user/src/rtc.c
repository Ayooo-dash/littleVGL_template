#include "rtc.h"
#include "delay.h"
#include "led.h"
#include "usart.h"

//RTC时间设置
ErrorStatus RTC_Time_Setting(u8 hour, u8 min, u8 sec, u8 ampm)
{
    RTC_TimeTypeDef RTC_TimeInitStructure;
    RTC_TimeInitStructure.RTC_H12 = ampm;
    RTC_TimeInitStructure.RTC_Hours = hour;
    RTC_TimeInitStructure.RTC_Minutes = min;
    RTC_TimeInitStructure.RTC_Seconds = sec;
    return RTC_SetTime(RTC_Format_BIN, &RTC_TimeInitStructure);
}

//RTC日期设置
ErrorStatus RTC_Date_Setting(u8 year, u8 month, u8 date, u8 week)
{
    RTC_DateTypeDef RTC_DateInitStructure;
    RTC_DateInitStructure.RTC_Date = date;
    RTC_DateInitStructure.RTC_Month = month;
    RTC_DateInitStructure.RTC_WeekDay = week;
    RTC_DateInitStructure.RTC_Year = year;
    return RTC_SetDate(RTC_Format_BIN, &RTC_DateInitStructure);
}

/*RTC初始化*/
u8 RTC_Setting_Init(void)
{
    RTC_InitTypeDef RTC_InitStructure;
    u16 retry = 0x1fff;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //使能PWR时钟
    PWR_BackupAccessCmd(ENABLE);                        //使能后备寄存器访问
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x1004)
    {
        RCC_LSEConfig(RCC_LSE_ON); //开启LSE
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {
            retry++;
            delay_ms(10);
        }
        if (retry == 0)
            return 1;                              //开启LSE失败
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    //设置RTC时钟（RTCCLK）,选择LSE作为RTC时钟
        RCC_RTCCLKCmd(ENABLE);                     //使能RTC时钟
        RTC_InitStructure.RTC_AsynchPrediv = 0x7f; //RTC异步分频系数127+1（1~0x7f）
        RTC_InitStructure.RTC_SynchPrediv = 0xff;  //RTC同步分频系数255+1（0~0x7fff）
        RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
        RTC_Init(&RTC_InitStructure);

        RTC_Time_Setting(17, 54, 55, RTC_H12_AM);
        RTC_Date_Setting(20, 7, 6, 1);

        RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1004); //初始化标志
    }
    return 0;
}

/*设置闹钟时间*/
void RTC_Alarm_Setting(u8 week, u8 hour, u8 min, u8 sec)
{
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE); //关闭闹钟A
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RTC_AlarmTypeDef RTC_AlarmInitStructure;
    RTC_TimeTypeDef RTC_TimeInitStructure;

    //闹钟时间配置
    RTC_TimeInitStructure.RTC_H12 = RTC_H12_AM;
    RTC_TimeInitStructure.RTC_Hours = hour;
    RTC_TimeInitStructure.RTC_Minutes = min;
    RTC_TimeInitStructure.RTC_Seconds = sec;
    RTC_AlarmInitStructure.RTC_AlarmDateWeekDay = week;
    RTC_AlarmInitStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
    RTC_AlarmInitStructure.RTC_AlarmMask = RTC_AlarmMask_None;
    RTC_AlarmInitStructure.RTC_AlarmTime = RTC_TimeInitStructure;
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmInitStructure);
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    EXTI_ClearITPendingBit(EXTI_Line17);
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

    //EXTI配置
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //NVIC配置
    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*周期性唤醒定时器配置*/
void RTC_Wakeup_Setting(u32 wksel, u16 arr)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RTC_WakeUpCmd(DISABLE);
    RTC_WakeUpClockConfig(wksel); //唤醒时钟选择
    RTC_SetWakeUpCounter(arr);    //设置WAKEUP自动重装载寄存器
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    RTC_WakeUpCmd(ENABLE);

    //EXTI配置
    EXTI_InitStructure.EXTI_Line = EXTI_Line22;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStructure);

    //NVIC配置
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*RTC闹钟中断服务函数*/
void RTC_Alarm_IRQHandler(void)
{
    if (RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)
    {
        RTC_ClearFlag(RTC_FLAG_ALRAF);
        printf("ALARM A!\r\n");
    }
    EXTI_ClearITPendingBit(EXTI_Line17);
}

/*RTC WAKE UP中断服务函数*/
void RTC_WKUP_IRQHandler(void)
{
    if (RTC_GetFlagStatus(RTC_FLAG_WUTF) == SET)
    {
        RTC_ClearFlag(RTC_FLAG_WUTF);
        DS1 = !DS1;
    }
    EXTI_ClearITPendingBit(EXTI_Line22);
}
