#ifndef __LOW_POWER_H
#define __LOW_POWER_H
#include "gd32f4xx.h"
#include "systick.h"
typedef enum {
    LP_MODE_SLEEP = 0,      // 睡眠模式（CPU 停，外设跑）
    LP_MODE_DEEPSLEEP,      // 深度睡眠模式（内核停，SRAM 保留）
    LP_MODE_STANDBY         // 待机模式（全部停，唤醒后复位）
} low_power_mode_t;

// 深度睡眠 LDO 配置 
typedef enum {
    LP_LDO_NORMAL = 0,      // LDO 正常模式（唤醒快）
    LP_LDO_LOWPOWER         // LDO 低功耗模式（更省电）
} lp_ldo_t;

//深度睡眠低驱动配置 
typedef enum {
    LP_LOWDRIVER_DISABLE = 0,   // 禁用低驱动
    LP_LOWDRIVER_ENABLE         // 使能低驱动（更省电）
} lp_lowdriver_t;

// 唤醒源标志
#define LP_WAKEUP_BY_WKUP_PIN   (1 << 0)   // WKUP 引脚（PA0）唤醒
#define LP_WAKEUP_BY_NRST       (1 << 1)   // NRST 复位唤醒
#define LP_WAKEUP_BY_RTC_ALARM  (1 << 2)   // RTC 闹钟唤醒
#define LP_WAKEUP_BY_IWDG       (1 << 3)   // IWDG 唤醒

#define STANDBY_FLAG_ADDR   RTC_BKP0
#define STANDBY_FLAG_SET    0xA5A5   // 需要进入待机
#define STANDBY_FLAG_CLEAR  0x0000   // 不需要进入待机

void LP_Init(void);
void LP_Enter_Sleep(void);
void LP_Enter_Deepsleep(lp_ldo_t ldo, lp_lowdriver_t lowdriver);
void LP_Enter_Standby(void);
void lp_wakeup_pin_enable(void);
void lp_wakeup_pin_disable(void);
uint8_t LP_Get_Wakeup_Flag(void);
void LP_Clear_Wakeup_Flag(void);
void  LP_Clock_Recover(void);

void LP_Set_Standby_Flag(void);
void LP_Clear_Standby_Flag(void);
uint8_t LP_Get_Standby_Flag(void);

#endif 

