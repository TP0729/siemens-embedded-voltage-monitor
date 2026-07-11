#ifndef  _LED_H
#define  _LED_H
#include "gd32f4xx.h"

#define 	GPIO_LED  	GPIOC

#define 	LED_Pin_0  	GPIO_PIN_6
#define 	LED_Pin_1  	GPIO_PIN_7
#define 	LED_Pin_2  	GPIO_PIN_8
#define 	LED_Pin_3  	GPIO_PIN_9
#define 	LED_Pin_4  	GPIO_PIN_10
#define 	LED_Pin_5  	GPIO_PIN_11



void LED_Config(void);
void LED_6(uint8_t x);
void LED_5(uint8_t x);
void LED_4(uint8_t x);
void LED_3(uint8_t x);
void LED_2(uint8_t x);
void LED_1(uint8_t x);
#endif
