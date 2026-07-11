#include "KEY.h"

void key_exti_config(uint32_t port, uint32_t pin, uint8_t exti_source_port, uint8_t exti_source_pin)
{
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_SYSCFG);
    gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
    syscfg_exti_line_config(exti_source_port, exti_source_pin);
    nvic_irq_enable(EXTI10_15_IRQn, 2U, 0U);
}


void key_init_all(void)
{
    key_exti_config(KEY_PORT,KEY_PIN1,KEY_SOURCE_PORT,KEY_SOURCE_PIN1);
	exti_init(EXTI_10, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_10);
	
    key_exti_config(KEY_PORT,  KEY_PIN2,KEY_SOURCE_PORT,  KEY_SOURCE_PIN2);
	exti_init(EXTI_11, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_11);
    
	key_exti_config(KEY_PORT,  KEY_PIN3, KEY_SOURCE_PORT,  KEY_SOURCE_PIN3);
	exti_init(EXTI_12, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_12);
	
    key_exti_config(KEY_PORT,  KEY_PIN4, KEY_SOURCE_PORT,  KEY_SOURCE_PIN4);
	exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_13);
	
	key_exti_config(KEY_PORT,  KEY_PIN5,KEY_SOURCE_PORT,  KEY_SOURCE_PIN5);
	exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_14);
	
    key_exti_config(KEY_PORT,  KEY_PIN6, KEY_SOURCE_PORT,  KEY_SOURCE_PIN6);
	exti_init(EXTI_15, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_15);
}

//void EXTI10_15_IRQHandler(void)
//{	
//	uint16_t key_status = gpio_input_port_get(GPIOD);
//	if ((key_status & (1<<15)) == 0)
//	{ 
//		gpio_bit_toggle(GPIOC,LED_Pin_5);
//		exti_interrupt_flag_clear(EXTI_15);//
//	}
//	else if ((key_status & (1<<14)) == 0)
//	{ 
//		gpio_bit_toggle(GPIOC,LED_Pin_4);
//		exti_interrupt_flag_clear(EXTI_14);//
//	}
//	else if ((key_status & (1<<13)) == 0)
//	{ 
//		gpio_bit_toggle(GPIOC,LED_Pin_3);
//		exti_interrupt_flag_clear(EXTI_13);//
//	}
//	if ((key_status & (1<<12)) == 0)
//	{ 
//		gpio_bit_toggle(GPIOC,LED_Pin_2);
//		exti_interrupt_flag_clear(EXTI_12);//
//	}
//	else if ((key_status & (1<<11)) == 0)
//	{ 
//		gpio_bit_toggle(GPIOC,LED_Pin_1);
//		exti_interrupt_flag_clear(EXTI_11);//
//	}
//	else if ((key_status & (1<<10)) == 0)
//	{ 
//		gpio_bit_toggle(GPIOC,LED_Pin_0);
//		exti_interrupt_flag_clear(EXTI_10);//
//	}
//}	

