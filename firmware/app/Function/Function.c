#include "Function.h"


void mcu_reset(void)
{
    __disable_irq();        // 关中断，避免复位时出岔子
    NVIC_SystemReset();     // 触发系统复位
}


float Get_ADC_Final_value(float ch0b)
{
	float value;
	float v;
	v = PC0_Vol_RV();;
	value = v*ch0b;
	return value;
}	

