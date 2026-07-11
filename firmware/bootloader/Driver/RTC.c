#include "RTC.h"

//#define RTC_CLOCK_SOURCE_LXTAL		// 配置RTC时钟源
#define BKP_VALUE    0x32F0
#define RTC_CLOCK_SOURCE_IRC32K
rtc_parameter_struct  	 rtc_initpara;
rtc_parameter_struct 	 rtc_current_time;

rtc_alarm_struct 		 rtc_alarm;
__IO uint32_t 			 prescaler_a = 0, prescaler_s = 0;
uint32_t 				 RTCSRC_FLAG = 0;

static uint8_t timer_state_c,timer_state_b;


void rtc_pre_config(void)
{
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();    
    rcu_osci_on(RCU_LXTAL);
    
    while(rcu_osci_stab_wait(RCU_LXTAL) != SUCCESS);
    
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();

    prescaler_s = 0xFF;
    prescaler_a = 0x7F;
}


void RTC_Init(void)
{
	rtc_parameter_struct rtc_para;
    rtc_pre_config();
	
    rtc_para.factor_asyn = prescaler_a;
    rtc_para.factor_syn  = prescaler_s;
    rtc_para.year   = 0x26;
    rtc_para.month  = 0x06;
    rtc_para.date   = 0x02;
    rtc_para.day_of_week = RTC_THURSDAY;
    rtc_para.hour   = 0x17;
    rtc_para.minute = 0x47;
    rtc_para.second = 0x20;
    rtc_para.display_format = RTC_24HOUR;
    rtc_para.am_pm  = RTC_AM;

    rtc_init_mode_enter();
    rtc_init(&rtc_para);
    rtc_init_mode_exit();
    rtc_register_sync_wait();
    RTC_BKP0 = BKP_VALUE;
}

void rtc_setup(void)
{
	rtc_parameter_struct rtc_initpara;
	rtc_parameter_struct rtc_time;
	rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
	
	// 以 LXTAL 为例
	rcu_osci_on(RCU_LXTAL);              // 开启外部低速晶振
	rcu_osci_stab_wait(RCU_LXTAL);       // 等待晶振稳定
	rcu_rtc_clock_config(RCU_RTCSRC_LXTAL); // 选择 LXTAL 作为 RTC 时钟
    /* setup RTC time value */
	
	rcu_periph_clock_enable(RCU_RTC);    // 使能 RTC 模块时钟
	rtc_register_sync_wait();            // 等待 RTC 寄存器与 APB 时钟同步
	
    pmu_backup_write_enable();
	rtc_register_sync_wait();  
	
	rtc_init_mode_enter();               // 设置 RTC_ISR 的 INIT 位	
	rtc_initpara.factor_asyn = 0x63;     // PREDIV_A
	rtc_initpara.factor_syn  = 0x7FFF;     // PREDIV_S
	rtc_init(&rtc_initpara);             // 写入 RTC_PRER
	rtc_init_mode_exit();      
	rtc_register_sync_wait();
   
	rtc_time.year   = 0x26;   // 2026 年，BCD 格式
	rtc_time.month  = 0x05;   // 5 月
	rtc_time.date   = 0x28;   // 28 日
	rtc_time.day_of_week = RTC_THURSDAY;
	rtc_time.hour   = 0x15;   // 15 时
	rtc_time.minute = 0x40;   // 30 分
	rtc_time.second = 0x00;   // 45 秒
	rtc_time.display_format = RTC_24HOUR;
	rtc_time.am_pm  = RTC_AM; // 24 小时制忽略

	rtc_init(&rtc_time);       // 写入 RTC_TR 和 RTC_DR
	rtc_init_mode_exit();      // 清除 RTC_ISR 的 INIT 位
	rtc_register_sync_wait();  // 等待 RSF 位置 1
} 

void rtc_get_time(rtc_parameter_struct *time)
{
    rtc_register_sync_wait();        // 必须等待同步
    rtc_current_time_get(time);      // 读取时间到结构体
}


uint32_t BCD_DEC(uint32_t bcd)
{
    uint32_t dec = 0;
    uint32_t factor = 1;
    while (bcd) 
	{
        dec += (bcd & 0x0F) * factor;
        bcd >>= 4;
        factor *= 10;
    }
    return dec;
}

void OLED_Show_HZtime(void)
{
	OLED_ShowChinese(32,0,3,16,1); //年
	OLED_ShowChinese(64,0,4,16,1);//月
	OLED_ShowChinese(96,0,5,16,1);//日
//	OLED_ShowChinese(70,16,6,16,1);//时
//	OLED_ShowChinese(90,16,7,16,1);//分
//	OLED_ShowChinese(110,16,8,16,1);//秒
//	OLED_ShowNum(0,2,20,0,16,1);

}	


void OLED_Show_colon(void)
{
//	OLED_ShowChar(32,0,':',16,1);
//	OLED_ShowChar(56,0,':',16,1);
//	
	OLED_ShowChar(32,16,':',16,1);
	OLED_ShowChar(56,16,':',16,1);
}	


void OLED_Show_timer(void)
{	
        rtc_current_time_get(&rtc_current_time);
	
		rtc_current_time.second = BCD_DEC(rtc_current_time.second);
		rtc_current_time.minute = BCD_DEC(rtc_current_time.minute);
		rtc_current_time.hour =   BCD_DEC(rtc_current_time.hour);
		
		rtc_current_time.date =  BCD_DEC(rtc_current_time.date);
		rtc_current_time.month = BCD_DEC(rtc_current_time.month);
		rtc_current_time.year =  BCD_DEC(rtc_current_time.year);
		
		timer_state_b = timer_state_c;
		timer_state_c = rtc_current_time.second;
		
	
	if(timer_state_c != timer_state_b)
	{
		OLED_ShowNum(16,2,rtc_current_time.year,2,16,1);
		OLED_ShowNum(48,2,rtc_current_time.month,2,16,1);
		OLED_ShowNum(80,2,rtc_current_time.date,2,16,1);
		
		OLED_ShowNum(16,18,rtc_current_time.hour,2,16,1);
		OLED_ShowNum(40,18,rtc_current_time.minute,2,16,1);
		OLED_ShowNum(64,18,rtc_current_time.second,2,16,1);
	}	
}








//    /* setup RTC alarm */
//    tmp_hh = 0xFF;
//    tmp_mm = 0xFF;
//    tmp_ss = 0xFF;

//    rtc_alarm_disable(RTC_ALARM0);
//    printf("=======Input Alarm Value=======\n\r");
//    rtc_alarm.alarm_mask = RTC_ALARM_DATE_MASK|RTC_ALARM_HOUR_MASK|RTC_ALARM_MINUTE_MASK;
//    rtc_alarm.weekday_or_date = RTC_ALARM_DATE_SELECTED;
//    rtc_alarm.alarm_day = 0x31;
//    rtc_alarm.am_pm = RTC_AM;

//    /* RTC alarm input */
//    printf("  please input Alarm Hour:\n\r");
//    while (0xFF == tmp_hh){
//        tmp_hh = usart_input_threshold(23);
//        rtc_alarm.alarm_hour = tmp_hh;
//    }
//    printf("  %0.2x\n\r", tmp_hh);

//    printf("  Please Input Alarm Minute:\n\r");
//    while (0xFF == tmp_mm){
//        tmp_mm = usart_input_threshold(59);
//        rtc_alarm.alarm_minute = tmp_mm;
//    }
//    printf("  %0.2x\n\r", tmp_mm);

//    printf("  Please Input Alarm Second:\n\r");
//    while (0xFF == tmp_ss){
//        tmp_ss = usart_input_threshold(59);
//        rtc_alarm.alarm_second = tmp_ss;
//    }
//    printf("  %0.2x", tmp_ss);

//    /* RTC alarm configuration */
//    rtc_alarm_config(RTC_ALARM0,&rtc_alarm);
//    printf("\n\r** RTC Set Alarm Success!  **\n\r");
//    rtc_show_alarm();

//    rtc_interrupt_enable(RTC_INT_ALARM0);
//    rtc_alarm_enable(RTC_ALARM0);






