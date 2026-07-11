#ifndef  _KEY_H
#define  _KEY_H
#include "gd32f4xx.h"


#define KEY_PORT        	GPIOD
#define KEY_IRQn    		EXTI10_15_IRQn 
#define KEY_RCU        		RCU_GPIOD
#define KEY_SOURCE_PORT 	EXTI_SOURCE_GPIOD

//personal designation
#define KEY_PIN1       		GPIO_PIN_10
#define KEY_EXTI_LINE1  	EXTI_10     
#define KEY_SOURCE_PIN1 	EXTI_SOURCE_PIN10

#define KEY_PIN2       		GPIO_PIN_11
#define KEY_EXTI_LINE2  	EXTI_11      
#define KEY_SOURCE_PIN2 	EXTI_SOURCE_PIN11

#define KEY_PIN3       		GPIO_PIN_12
#define KEY_EXTI_LINE3  	EXTI_12      
#define KEY_SOURCE_PIN3 	EXTI_SOURCE_PIN12

#define KEY_PIN4       		GPIO_PIN_13
#define KEY_EXTI_LINE4  	EXTI_13     
#define KEY_SOURCE_PIN4 	EXTI_SOURCE_PIN13

#define KEY_PIN5       		GPIO_PIN_14
#define KEY_EXTI_LINE5  	EXTI_14      
#define KEY_SOURCE_PIN5 	EXTI_SOURCE_PIN14

#define KEY_PIN6       		GPIO_PIN_15
#define KEY_EXTI_LINE6  	EXTI_15     
#define KEY_SOURCE_PIN6 	EXTI_SOURCE_PIN15


void key_exti_config(uint32_t port, uint32_t pin, uint8_t exti_source_port, uint8_t exti_source_pin); 

void key_init_all(void);











#endif
