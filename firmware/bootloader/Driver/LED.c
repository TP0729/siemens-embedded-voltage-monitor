#include "LED.h"

void LED_Config(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
   
    gpio_mode_set(GPIO_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_Pin_0|LED_Pin_1|LED_Pin_2|LED_Pin_3|LED_Pin_4|LED_Pin_5);
    gpio_output_options_set(GPIO_LED, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_Pin_0|LED_Pin_1|LED_Pin_2|LED_Pin_3|LED_Pin_4|LED_Pin_5);

    gpio_bit_reset(GPIO_LED , LED_Pin_0|LED_Pin_1|LED_Pin_2|LED_Pin_3|LED_Pin_4|LED_Pin_5);
}	

void LED_6(uint8_t x)
{
	if(x)  gpio_bit_set(GPIO_LED, LED_Pin_0);
	else gpio_bit_reset(GPIO_LED, LED_Pin_0);
}

void LED_5(uint8_t x)
{
	if(x)  gpio_bit_set(GPIO_LED, LED_Pin_1);
	else gpio_bit_reset(GPIO_LED, LED_Pin_1);
}


void LED_4(uint8_t x)
{
	if(x)  gpio_bit_set(GPIO_LED, LED_Pin_2);
	else gpio_bit_reset(GPIO_LED, LED_Pin_2);
}

void LED_3(uint8_t x)
{
	if(x)  gpio_bit_set(GPIO_LED, LED_Pin_3);
	else gpio_bit_reset(GPIO_LED, LED_Pin_3);
}


void LED_2(uint8_t x)
{
	if(x)  gpio_bit_set(GPIO_LED, LED_Pin_4);
	else gpio_bit_reset(GPIO_LED, LED_Pin_4);
}

void LED_1(uint8_t x)
{
	if(x)  gpio_bit_set(GPIO_LED, LED_Pin_5);
	else gpio_bit_reset(GPIO_LED, LED_Pin_5);
}






