#include "ADC.h"

float V;
static float V_C,V_B;
void ADC_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC0);
  
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV8);  // 假设 APB2=120MHz, /8=15MHz
    
    adc_deinit();
    
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_resolution_config(ADC0, ADC_RESOLUTION_12B);
   
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);
    
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_15);
    
    adc_enable(ADC0);
    delay_1ms(10);
    adc_calibration_enable(ADC0);
}


uint16_t Get_adc_value(void)
{
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
    while (adc_flag_get(ADC0, ADC_FLAG_EOC) == RESET);
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    return adc_routine_data_read(ADC0);
}

float ADC_Volatge(uint16_t ADC_VALUE)//将getadcvalue转化位实际值
{
	float v;
	v = (3.3f/4095.0f)*(float)ADC_VALUE;
	return v;
}


float PC0_Vol_RV(void)//结算PC0角实际值
{
	uint16_t i;
	float v;
	i = Get_adc_value();
	v = ADC_Volatge(i);
	return v;
}	


void ADC_OLED_Init(void)
{	
	OLED_ShowChinese(20,8,19,16,1);//ADC
	OLED_ShowChinese(36,8,20,16,1);//
	OLED_ShowString(82,8,"V",16,1);//采集部分
}

void OLED_ADC_Show_V(void)
{
	V = PC0_Vol_RV();
	V_B = V_C;
	V_C = V;
	if(V_C != V_B) OLED_ShowFloat(50,8,V,2,16,1);
}	




