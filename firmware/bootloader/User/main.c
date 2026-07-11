#include "All_HeadFile.h"


__IO static uint8_t Jump_State = 0;


int main(void)
{
	USART1_Init();
	usart_printf("A5B60001058888000263A1B6A5");
	
	LED_Config();
	timer5s_init();
//	
	
	OLED_Init();
	OLED_Clear();
	OLED_ShowNum(0,0,2026,4,16,1);//
	OLED_ShowNum(32,0,658075,6,16,1);
	OLED_ShowString(0,16,"Bootloader",16,1);
	
	 
	

	while(1)
	{

		if(Jump_State == 2)
		{
			 timer_disable(TIMER2);
			//jump_to_app();
			JumpToApp();
		}
		
		

     }
}

void TIMER2_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_UP) == SET)
    {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);
       Jump_State ++;
    }
}
