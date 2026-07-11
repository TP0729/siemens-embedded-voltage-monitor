#ifndef  _ALL_HEADFILE_H
#define  _ALL_HEADFILE_H
#include "gd32f4xx.h"
//sys
#include "gd32f4xx.h"
#include "gd32f470v_start.h"
#include "systick.h"
#include <stdio.h>
#include <math.h>
//Hardware
#include "LED.h"
#include "KEY.h"
#include "oled.h"
#include "ADC.h"
#include "RTC.h"
#include "DAC.h"
#include "USART.h"
#include "SPI_FLASH.h"
#include "PWU.h"
#include "Delay.h"
#include "SPI_AD3344.h"
#include "TIMER.h"
#include "Protocol.h"
#include "Function.h"




#endif
//if(T == 4)
//		{
//			T =0;
//			if(RE_RES==0)
//			{
//				usart_printf("A5B6 0001 02 0221 04 02 421A7400 FC57 B6A5");
//				RE_RES=1;
//			}
//			else  if(RE_RES==1)
//			{					 
//				usart_printf("A5B6 0001 02 0221 04 02 421CE1 FC57 B6A5");
//			}
//			delay_1ms(1);
//			gpio_bit_reset(GPIOE, GPIO_PIN_8);
//		}	

