#include "pwu.h"

static uint8_t wakeup_flags = 0;


void LP_Init(void)
{
    rcu_periph_clock_enable(RCU_PMU);
    
    /* 清除残留唤醒标志 */
     LP_Clear_Wakeup_Flag();
}

/* 进入睡眠模式（任意中断即可唤醒）*/
void LP_Enter_Sleep(void)
{
    /* 使用标准库函数进入睡眠模式 */
    pmu_to_sleepmode(WFI_CMD);
}

/* 进入深度睡眠模式 */
void LP_Enter_Deepsleep(lp_ldo_t ldo, lp_lowdriver_t lowdriver)
{
    uint32_t ldo_cfg;
    uint32_t driver_cfg;
    
    ldo_cfg = (ldo == LP_LDO_LOWPOWER) ? PMU_LDO_LOWPOWER : PMU_LDO_NORMAL;
    driver_cfg = (lowdriver == LP_LOWDRIVER_ENABLE) ? PMU_LOWDRIVER_ENABLE : PMU_LOWDRIVER_DISABLE;
   
     LP_Clear_Wakeup_Flag();
   
    pmu_to_deepsleepmode(ldo_cfg, driver_cfg, WFI_CMD);
    
    LP_Clock_Recover();
}

// 进入待机模式
void LP_Enter_Standby(void)
{
    LP_Clear_Wakeup_Flag();
    pmu_to_standbymode();
    while(1);
}
//用不到的
void lp_wakeup_pin_enable(void)
{
    rcu_periph_clock_enable(RCU_PMU);
    pmu_wakeup_pin_enable();
    wakeup_flags |= LP_WAKEUP_BY_WKUP_PIN;
}
void lp_wakeup_pin_disable(void)
{
    rcu_periph_clock_enable(RCU_PMU);
    pmu_wakeup_pin_disable();
    wakeup_flags &= ~LP_WAKEUP_BY_WKUP_PIN;
}


uint8_t LP_Get_Wakeup_Flag(void)
{
    uint8_t flags = 0;
    
    // 检查 PMU 唤醒标志 
    if (pmu_flag_get(PMU_FLAG_WAKEUP)) {
        flags |= LP_WAKEUP_BY_WKUP_PIN;
    }
    
    // 检查待机标志 
    if (pmu_flag_get(PMU_FLAG_STANDBY)) {
        flags |= LP_WAKEUP_BY_NRST;
    }
    
    // 合并用户设置的唤醒源
    flags |= wakeup_flags;
    
    return flags;
}


void LP_Clear_Wakeup_Flag(void)
{
    pmu_flag_clear(PMU_FLAG_RESET_WAKEUP);
    pmu_flag_clear(PMU_FLAG_RESET_STANDBY);
}

// 深度睡眠唤醒后辉复系统时钟
void LP_Clock_Recover(void)
{
    rcu_osci_on(RCU_HXTAL);
    while (rcu_osci_stab_wait(RCU_HXTAL) != SUCCESS);

    rcu_pll_config(RCU_PLLSRC_HXTAL, 0, 15, 2, 7);
    
    /* 3. 等待 PLL 锁定 */
    rcu_osci_on(RCU_PLL_CK);
    while (rcu_osci_stab_wait(RCU_PLL_CK) != SUCCESS);
    
    /* 4. 选择 PLL 作为系统时钟 */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLLP);
    
    /* 5. 等待切换完成 */
    while (rcu_system_clock_source_get() != RCU_SCSS_PLLP);
    
    /* 6. 重新配置 SysTick（如果用了的话）*/
    systick_config();
}

void LP_Set_Standby_Flag(void)
{
    pmu_backup_write_enable();       // 使能备份域写入
    RTC_BKP0 = STANDBY_FLAG_SET;
}

void LP_Clear_Standby_Flag(void)
{
    pmu_backup_write_enable();
    RTC_BKP0 = STANDBY_FLAG_CLEAR;
}

uint8_t LP_Get_Standby_Flag(void)
{
    return (RTC_BKP0 == STANDBY_FLAG_SET);
}


