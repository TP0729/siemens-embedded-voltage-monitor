#include "TIMER.h"
// 定时器初始化函数
void timer5s_init(void)
{
	timer_parameter_struct timer_initpara;
    // 1. 使能TIMER2时钟
    rcu_periph_clock_enable(RCU_TIMER2);

    // 2. 复位定时器（可选，确保干净状态）
    timer_deinit(TIMER2);

    // 3. 配置定时器参数
   
    timer_struct_para_init(&timer_initpara);

    // 系统时钟 240MHz，TIMER2 时钟通常等于系统时钟（若APB1分频为1）
    // 要得到1秒中断，计算公式: (prescaler+1)*(period+1) = TimerClock
    // 240,000,000 = 24000 * 10000
    timer_initpara.prescaler         = 12000 - 1;    // 分频系数 23999
    timer_initpara.period            = 50000 - 1;    // 自动重装值 9999
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);

    // 4. 使能定时器更新中断
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    // 5. 配置NVIC中断优先级
    nvic_irq_enable(TIMER2_IRQn, 3, 0);   // 抢占优先级1，子优先级0（可调整）

    // 6. 启动定时器
    timer_enable(TIMER2);
}

// 定时器2中断服务函数


