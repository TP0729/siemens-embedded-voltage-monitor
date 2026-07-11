#ifndef __RTC_H
#define __RTC_H

#include "ALL_HeadFile.h"


void rtc_pre_config(void);
void RTC_Init(void);
void rtc_setup(void);
void rtc_get_time(rtc_parameter_struct *time);
uint32_t BCD_DEC(uint32_t bcd);
void OLED_Show_timer(void);
void OLED_Show_colon(void);
void OLED_Show_HZtime(void);

#endif
