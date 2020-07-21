#ifndef __RTC_H__
#define __RTC_H__
#include "stm32f4xx.h"

ErrorStatus RTC_Time_Setting(u8 hour, u8 min, u8 sec, u8 ampm);
ErrorStatus RTC_Date_Setting(u8 year, u8 month, u8 date, u8 week);
u8 RTC_Setting_Init(void);
void RTC_Alarm_Setting(u8 week, u8 hour, u8 min, u8 sec);
void RTC_Wakeup_Setting(u32 wksel, u16 arr);

#endif

