#include "All_HeadFile.h"

float ADCV;
extern uint32_t B_rate;
uint32_t IEEE_ADC;
uint8_t data,FLASH[256],READ[256];

// systick_config();
//static uint8_t sleep_state;

static uint32_t ID = 0x0001;
//ADC3344
int16_t adc_16bit,b_rate;
uint16_t register_data;
float ADC_Value;
float pga;
uint8_t i;
uint32_t sample_count;
uint32_t number_of_sample = 1;
static volatile uint8_t sample_no_eff_num = 255;


parsed_frame_t frame;//命令的解析


static uint8_t T;//采样温度,重复电阻实在买不到合适的精密电阻了

static uint16_t DAC_V = 2000;
float alarm_Thre_Ch0 = 3.2f,alarm_Thre_Ch1= 3.1f;


static float CH0_B = 1.0f,CH1_B = 1.0f;
uint8_t CRC_FR[] = {0};
uint16_t Curren_CRC;
float V_ch1;

uint32_t GD25ID;


config_t GD25,GD25data;


int main(void)
{
	
	
    SCB->VTOR = 0x08011000; // 指向你的APP起始地址

		
	LED_Config();
	timer1s_init();
	//key_init_all();
	//RTC_Init();
	//LP_Init();
	//lp_wakeup_pin_enable();

	  
	
	//RTC_Init();
	
	
	OLED_Init();
	OLED_Clear();
	OLED_ShowNum(0,0,2026,4,16,1);//
	OLED_ShowNum(32,0,658075,6,16,1);
	
	OLED_ShowString(0,16,"IDLE",16,1);
	
	//OLED_Clear();
//	OLED_Show_lace();
//	OLED_Show_colon();
//	OLED_Show_HZtime();
	//485
	
//usart_printf("A5B6%xFFEEEE0002E087B6A5",ID);
	//ADC
	ADC_Init();
	//ADCV = Get_ADC_Final_value(CH0_B);
	//IEEE_ADC = float_to_hex(ADCV);
	
	
	//OLED_ShowHexNum(64,16,DAC_V,8,16,1);
		DAC_Init();
	//DAC_V = CH1_F_To_H(CH1_B);
	 DAC_Set_Output(DAC_V);
	 V_ch1 = ((float)(DAC_V/4096.0f))*3.3f*CH1_B;
	//OLED_ShowFloat(48,16,V_ch1,2,16,1);
//	   GD25.device_id = ID;
//		GD25.baudrate = 19200;
//		GD25.CH0_b = CH0_B;
//		GD25.CH1_b = CH1_B;
//		GD25.CH0_T = alarm_Thre_Ch0;
//		GD25.CH1_T = alarm_Thre_Ch1;
//		

		GD25_Init();
		
		//save_config_to_flash(&GD25);
		load_config_from_flash(&GD25data);
		
		B_rate = GD25data.baudrate;
		ID = GD25data.device_id;
		CH0_B = GD25data.CH0_b;
		CH1_B = GD25data.CH1_b;
		alarm_Thre_Ch0 = GD25data.CH0_T;
		alarm_Thre_Ch1 = GD25data.CH1_T;
		USART1_Init();
		
	
	//OLED_ShowHexNum(64,16,GD25data.device_id,4,16,1);
	//OLED_ShowFloat(32,16,GD25data.CH0_T,2,16,1);
//	ID = gd25_read_jedec_id();
//	usart_printf("%x",ID);
	
	//LP_Enter_Deepsleep(LP_LDO_NORMAL,LP_LOWDRIVER_DISABLE);

//	
//	if(!LP_Get_Standby_Flag())
//	{	
//		LP_Set_Standby_Flag();
//		LP_Enter_Standby();
//	}	
//	LP_Clear_Standby_Flag();

	//usart_printf("A5B6%xFFEEEE0002E087B6A5",ID);


	ad3344_spi_init();  
    ad3344_process();
    ad3344_ExtRef();
   
   AD3344_reg_Config(AD3344_SINGLE_END,0);

    ad3344_init(AD3344_CONFIG);

   ad3344_Exti_enable();
   usart_printf("A5B6 %x 05 8888 00 02 63A1 B6A5",ID);
   delay_1ms(1);
					gpio_bit_reset(GPIOE, GPIO_PIN_8);
					
	while(1)
	{
		
		T = External_ADC_Sample_T(T,ID);

		if(parse_received_frame(&frame))
		{
			if(frame.error == 1)
			{
					usart_printf("A5B6%xFFEEEE0002E087B6A5",ID);
					delay_1ms(1);
					gpio_bit_reset(GPIOE, GPIO_PIN_8);
			}	
			
			else if( frame.device_id == 0xFFFF)
			{
				
				uint8_t Frame[11] = {0xA5, 0xB6, 0x00, 0x01, 0x02, 0x01, 0x11, 0x02, 0x02, 0x00,0x00};
				uint16_t crc ;
				uint8_t i,d;
				i = ((ID>>8)&0xFF);
				d = (ID&0xFF);
				Frame[2] =i;
				Frame[3] = d;
				Frame[9] =i;
				Frame[10] = d;		
				crc = CRC16_modbus(Frame, sizeof(Frame));
				
				usart_printf("A5B6 %x 02 0111 02 02 %x %x B6A5",ID,ID,crc);//A5B6 0002 02 0111 02 02 0002 508C B6A5 
				delay_1ms(1);
				gpio_bit_reset(GPIOE, GPIO_PIN_8);
			}	
			else 
			{	
			  if(frame.device_id == ID)
			  {
				if( frame.frame_type == 0x01 )
				{
				
					if(frame.command == 0x0221)
					{  
						ad3344_Exti_enable();
					}
					if(frame.command == 0x0101)//重启
					{  
						delay_s(2);
						mcu_reset();;
					}	
					
					if(frame.command == 0x0104)
					{  
						usart_printf("A5B6 %x 02 0104 04 02 00 01 00 04 9721 B6A5 ",ID);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					}	
					
					if(frame.command == 0x01A1)//修改ID
					{  
						uint8_t Frame[10] = {0xA5, 0xB6, 0x00, 0x01, 0x02, 0x01, 0xA1, 0x01, 0x02, 0xFF};
						uint16_t crc ;
						uint8_t i,d;
						i = frame.data[0] ;
						d = frame.data[1] ;
						Frame[2] =i;
						Frame[3] = d;
						ID = ((uint16_t)frame.data[0] << 8) | ((uint16_t)frame.data[1]);
						
						GD25.device_id = ID;//保存FLASH
						
							GD25.CH0_b = CH0_B;
							GD25.CH1_b = CH1_B;
							GD25.CH0_T = alarm_Thre_Ch0;
							GD25.CH1_T = alarm_Thre_Ch1;
							GD25.baudrate = B_rate ;
						save_config_to_flash(&GD25);
						
						//OLED_ShowHexNum(64,16,ID,4,16,1);
						crc = CRC16_modbus(Frame, sizeof(Frame));
						usart_printf("A5B6 %x 02 01A1 01 02 FF %x B6A5 ",ID,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						gpio_bit_toggle(GPIO_LED, LED_Pin_3);
					}	
					if(frame.command == 0x0106)//发送时间戳
					{  
						uint8_t Frame[13] = {0xA5, 0xB6, 0x00, 0x01, 0x02, 0x01, 0x06, 0x04, 0x02, 0, 0, 0, 0};
						uint16_t crc ;
						uint8_t HH,HL,LH,LL,ID_H,ID_L;
						uint16_t High,Low;
						ID_H = (ID >> 8) & 0xFF; 
						ID_L = ID & 0xFF;
					
						
						IEEE_ADC = Get_Current_timestamp();
						High = (uint16_t)(IEEE_ADC >> 16);      // 高16位 = 0x41AC
						Low  = (uint16_t)(IEEE_ADC & 0xFFFF);   // 低16位 = 0xB852

						 HH = (High >> 8) & 0xFF;        // 高字节 0x41
						 HL = High & 0xFF;               // 低字节 0xAC
						 LH = (Low >> 8) & 0xFF;         // 高字节 0xB8
						 LL = Low & 0xFF;                // 低字节 0x52
							
						Frame[2] =ID_H;
						Frame[3] = ID_L;
						Frame[9] =HH;
						Frame[10] = HL;
						Frame[11] =	LH;
						Frame[12] =LL;
						crc = CRC16_modbus(Frame, sizeof(Frame));
						usart_printf("A5B6 0001 02 0106 04 02 %x %x %x B6A5 ",High,Low,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					}	
					if(frame.command == 0x0105)//接受时间戳
					{  
						 rtc_parameter_struct rtc;
						
						uint32_t timestamp = ((uint32_t)frame.data[0] << 24) |
                     ((uint32_t)frame.data[1] << 16) |
                     ((uint32_t)frame.data[2] << 8)  |
                     ((uint32_t)frame.data[3]);
						rtc_parameter_struct rtc_para;
						timestamp_to_rtc_struct(timestamp,&rtc);
					
					rtc_pre_config();
					
					rtc_para.factor_asyn = 0x7F;
					rtc_para.factor_syn  = 0xFF;
					rtc_para.year   = rtc.year;
					rtc_para.month  = rtc.month;
					rtc_para.date   = (rtc.date);
					//rtc_para.day_of_week = RTC_THURSDAY;
					rtc_para.hour   = (rtc.hour);
					rtc_para.minute = (rtc.minute);
					rtc_para.second = (rtc.second);
					rtc_para.display_format = RTC_24HOUR;
					rtc_para.am_pm  = RTC_AM;

					rtc_init_mode_enter();
					rtc_init(&rtc_para);
					rtc_init_mode_exit();
					rtc_register_sync_wait();
					RTC_BKP0 = 0x32F0;	
						timestamp_to_rtc_struct(timestamp,&rtc);
					
						usart_printf("A5B6 %x 02 0105 01 02 FF 3C44 B6A5 ",ID);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					}	
					
					if(frame.command == 0x0112)//波特率//////////////////////////////////////////////////////////////////////////////
					{  
						if(B_rate == 19200) 		usart_printf("A5B6 0001 02 0112 01 02 13 0540 B6A5");
						else if(B_rate == 4800)		usart_printf("A5B6 0001 02 0112 01 02 11 C4C1 B6A5");
						else if(B_rate == 9600) 	usart_printf("A5B6 0001 02 0112 01 02 12 C581 B6A5");
						else if(B_rate == 115200)   usart_printf("A5B6 0001 02 0112 01 02 14 C701 B6A5");
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					}	
					if(frame.command == 0x0400)
					{  
						usart_printf("A5B6 0001 02 0104 04 02 00 01 00 04 9721 B6A5 ");
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						//gpio_bit_toggle(GPIO_LED, LED_Pin_3);
					}	
					if(frame.command == 0x0401||frame.command == 0x0400)//读取电位器阈值
					{  
						uint8_t frame[13] = {0xA5, 0xB6, 0x00, 0x01, 0x02, 0x02, 0x02, 0x04, 0x02, 0, 0, 0, 0};
						uint16_t crc ;
						uint8_t HH,HL,LH,LL;
						uint16_t High,Low;
						V_ch1 = alarm_Thre_Ch0;
						IEEE_ADC = float_to_hex(V_ch1);
						
						High = (uint16_t)(IEEE_ADC >> 16);      // 高16位 = 0x41AC
						Low  = (uint16_t)(IEEE_ADC & 0xFFFF);   // 低16位 = 0xB852

						 HH = (High >> 8) & 0xFF;        // 高字节 0x41
						 HL = High & 0xFF;               // 低字节 0xAC
						 LH = (Low >> 8) & 0xFF;         // 高字节 0xB8
						 LL = Low & 0xFF;                // 低字节 0x52

						frame[9] =HH;
						frame[10] = HL;
						frame[11] =	LH;
						frame[12] =LL;
						crc = CRC16_modbus(frame, sizeof(frame));
						usart_printf("A5B6 0001 02 0202 04 02 %x %x %x B6A5 ",High,Low,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					}
					if(frame.command == 0x0402||frame.command == 0x0400)//读取电位器
					{  
						uint8_t frame[13] = {0xA5, 0xB6, 0x00, 0x01, 0x02, 0x02, 0x02, 0x04, 0x02, 0, 0, 0, 0};
						uint16_t crc ;
						uint8_t HH,HL,LH,LL;
						uint16_t High,Low;
						V_ch1 = alarm_Thre_Ch1;
						IEEE_ADC = float_to_hex(V_ch1);
						
						High = (uint16_t)(IEEE_ADC >> 16);      // 高16位 = 0x41AC
						Low  = (uint16_t)(IEEE_ADC & 0xFFFF);   // 低16位 = 0xB852

						 HH = (High >> 8) & 0xFF;        // 高字节 0x41
						 HL = High & 0xFF;               // 低字节 0xAC
						 LH = (Low >> 8) & 0xFF;         // 高字节 0xB8
						 LL = Low & 0xFF;                // 低字节 0x52

						frame[9] =HH;
						frame[10] = HL;
						frame[11] =	LH;
						frame[12] =LL;
						crc = CRC16_modbus(frame, sizeof(frame));
						usart_printf("A5B6 0001 02 0202 04 02 %x %x %x B6A5 ",High,Low,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					}
					
					if(frame.command == 0x0202)//CH1
					{  
						
						uint8_t frame[13] = {0xA5, 0xB6, 0x00, 0x01, 0x02, 0x02, 0x02, 0x04, 0x02, 0, 0, 0, 0};
						uint16_t crc ;
						uint8_t HH,HL,LH,LL;
						uint16_t High,Low;
						V_ch1 = ((float)(DAC_V/4096.0f))*3.3f*CH1_B;
						IEEE_ADC = float_to_hex(V_ch1);
						
						High = (uint16_t)(IEEE_ADC >> 16);      // 高16位 = 0x41AC
						Low  = (uint16_t)(IEEE_ADC & 0xFFFF);   // 低16位 = 0xB852

						 HH = (High >> 8) & 0xFF;        // 高字节 0x41
						 HL = High & 0xFF;               // 低字节 0xAC
						 LH = (Low >> 8) & 0xFF;         // 高字节 0xB8
						 LL = Low & 0xFF;                // 低字节 0x52

						frame[9] =HH;
						frame[10] = HL;
						frame[11] =	LH;
						frame[12] =LL;
						crc = CRC16_modbus(frame, sizeof(frame));
						usart_printf("A5B6 0001 02 0202 04 02 %x %x %x B6A5 ",High,Low,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						
					}	
					if(frame.command == 0x0201)//CH0
					{  
						uint8_t frame[13] = {0xA5, 0xB6, 0x00, 0x01, 0x02, 0x02, 0x01, 0x04, 0x02, 0, 0, 0, 0};
						uint16_t crc ;
						uint8_t HH,HL,LH,LL;
						uint16_t High,Low;
						ADCV = Get_ADC_Final_value(CH0_B);
						IEEE_ADC = float_to_hex(ADCV);          // 例如 0x41ACB852
						High = (uint16_t)(IEEE_ADC >> 16);      // 高16位 = 0x41AC
						Low  = (uint16_t)(IEEE_ADC & 0xFFFF);   // 低16位 = 0xB852

						 HH = (High >> 8) & 0xFF;        // 高字节 0x41
						 HL = High & 0xFF;               // 低字节 0xAC
						 LH = (Low >> 8) & 0xFF;         // 高字节 0xB8
						 LL = Low & 0xFF;                // 低字节 0x52

						frame[9] =HH;
						frame[10] = HL;
						frame[11] =	LH;
						frame[12] =LL;
						crc = CRC16_modbus(frame, sizeof(frame));
						usart_printf("A5B6 0001 02 0201 04 02 %x %x %x B6A5 ",High,Low,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						//gpio_bit_toggle(GPIO_LED, LED_Pin_3);
					}	
					
					if(frame.command == 0x0241)//CH0变比设置
					{  
						
						uint32_t hex = ((uint32_t)frame.data[0] << 24) |
                       ((uint32_t)frame.data[1] << 16) |
                       ((uint32_t)frame.data[2] << 8)  |
                       ((uint32_t)frame.data[3]);
						float ch0_ratio = hex_to_float(hex);
						CH0_B = ch0_ratio;
						
						GD25.CH0_b = CH0_B;//保存FLASH
						GD25.device_id = ID;//保存FLASH
						
							
							GD25.CH1_b = CH1_B;
							GD25.CH0_T = alarm_Thre_Ch0;
							GD25.CH1_T = alarm_Thre_Ch1;
						GD25.baudrate = B_rate ;
						save_config_to_flash(&GD25);
						
						
						usart_printf("A5B6 %x 02 0241 01 02 FF CC14 B6A5",ID);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						//gpio_bit_toggle(GPIO_LED, LED_Pin_3);
					}	
					if(frame.command == 0x0242)//CH1变比设置
					{  
						
						uint32_t hex = ((uint32_t)frame.data[0] << 24) |
                       ((uint32_t)frame.data[1] << 16) |
                       ((uint32_t)frame.data[2] << 8)  |
                       ((uint32_t)frame.data[3]);
						float ch0_ratio = hex_to_float(hex);
						CH1_B = ch0_ratio;
						
						GD25.CH1_b = CH1_B;//保存FLASH
						
						GD25.device_id = ID;//保存FLASH
						
							GD25.CH0_b = CH0_B;
							GD25.CH0_T = alarm_Thre_Ch0;
							GD25.CH1_T = alarm_Thre_Ch1;
						GD25.baudrate = B_rate ;
						save_config_to_flash(&GD25);
						
						usart_printf("A5B6 %x 02 0242 01 02 FF 8814 B6A5",ID);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						//gpio_bit_toggle(GPIO_LED, LED_Pin_3);
					}	
					if(frame.command == 0x0411)//CH0阈值设置
					{  
						
						uint32_t hex = ((uint32_t)frame.data[0] << 24) |
                       ((uint32_t)frame.data[1] << 16) |
                       ((uint32_t)frame.data[2] << 8)  |
                       ((uint32_t)frame.data[3]);
						float ch0_ratio = hex_to_float(hex);
						alarm_Thre_Ch0 = ch0_ratio;
						
						GD25.CH0_T = alarm_Thre_Ch0;//保存FLASH
						
						GD25.device_id = ID;//保存FLASH
						
							GD25.CH0_b = CH0_B;
							GD25.CH1_b = CH1_B;
							GD25.CH1_T = alarm_Thre_Ch1;
						GD25.baudrate = B_rate ;
						save_config_to_flash(&GD25);
						
						
						
						usart_printf("A5B6 %x 02 0411 01 02 FF CC8D B6A5",ID);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						//gpio_bit_toggle(GPIO_LED, LED_Pin_3);
					}
					if(frame.command == 0x0412)//CH1阈值设置
					{  
						
						uint32_t hex = ((uint32_t)frame.data[0] << 24) |
                       ((uint32_t)frame.data[1] << 16) |
                       ((uint32_t)frame.data[2] << 8)  |
                       ((uint32_t)frame.data[3]);
						float ch0_ratio = hex_to_float(hex);
						alarm_Thre_Ch1 = ch0_ratio;
						
						GD25.CH1_T = alarm_Thre_Ch1;//保存FLASH
						
						GD25.device_id = ID;//保存FLASH
						
							GD25.CH0_b = CH0_B;
							GD25.CH1_b = CH1_B;
							GD25.CH0_T = alarm_Thre_Ch0;
						GD25.baudrate = B_rate ;
						save_config_to_flash(&GD25);
						
						usart_printf("A5B6 %x 02 0412 01 02 FF 888D B6A5",ID);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
						//gpio_bit_toggle(GPIO_LED, LED_Pin_3);
					}
					if(frame.command == 0x0301)//设置DAC输出
					{
						
						
						uint8_t Frame[10] = {0xA5, 0xB6, 0x00, 0x01,0x02,0x03, 0x01, 0x01, 0x02, 0xFF};
						uint16_t crc ;
						uint8_t i,d;
						i = ((ID>>8)&0xFF);
						d = (ID&0xFF);
						Frame[2] =i;
						Frame[3] = d;
						crc = CRC16_modbus(Frame, sizeof(Frame));
						DAC_V = (frame.data[0] << 8) | frame.data[1];
						DAC_Set_Output(DAC_V);
						V_ch1 = ((float)(DAC_V/4096.0f))*3.3f*CH1_B;
						usart_printf("A5B6 %x 02 0301 01 02 FF %x B6A5",ID,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					
					
					}	
					if(frame.command == 0x01A2)//设置波特率输出
					{
						
						
						uint8_t Frame[10] = {0xA5, 0xB6, 0x00, 0x01,0x02,0x01, 0xA2, 0x01, 0x02, 0xFF};
						uint16_t crc ;
						uint8_t i,d;
						i = ((ID>>8)&0xFF);
						d = (ID&0xFF);
						Frame[2] =i;
						Frame[3] = d;
						crc = CRC16_modbus(Frame, sizeof(Frame));
						if(frame.data[0] == 11)        B_rate =4800;
						else if(frame.data[0] == 12)   B_rate =9600;
						else if(frame.data[0] == 13)   B_rate =19200;
						else if(frame.data[0] == 14)	B_rate =115200;
						GD25.baudrate = B_rate ;
						
						USART1_Init();	
						GD25.CH1_T = alarm_Thre_Ch1;//保存FLASH
						
						GD25.device_id = ID;//保存FLASH
						
						GD25.CH0_b = CH0_B;
						GD25.CH1_b = CH1_B;
						GD25.CH0_T = alarm_Thre_Ch0;
						
						save_config_to_flash(&GD25);
						usart_printf("A5B6 %x 02 01A2 01 02 FF %x B6A5",ID,crc);
						delay_1ms(1);
						gpio_bit_reset(GPIOE, GPIO_PIN_8);
					
					
					}	
					
					
						
				}	
				
				
				
				
				
				
				
				
				
				
				else if( frame.frame_type == 0x05)	{}
				else 
				{
					usart_printf("A5B6%xFFEEEE0002E087B6A5",ID);
					delay_1ms(1);
					gpio_bit_reset(GPIOE, GPIO_PIN_8);
				}	
			  }	
			}	

		
		}
		
		
		
		
	}	
	
	//delay_s(1);
	
	
	
}


float adcdata_to_volt(uint16_t bin)
{
    int16_t value = (int16_t)bin;          // 将无符号数转为有符号16位（编译器自动处理补码）
    return (float)value / 32768.0f * pga*1.6f;  // pga 为满量程电压（例如 2.048f）
}


void EXTI5_9_IRQHandler()
{
	uint8_t D;
    if(RESET != exti_interrupt_flag_get(EXTI_6))
    { exti_interrupt_flag_clear(EXTI_6);
        //
        adc_16bit = ad3344_read_data16(AD3344_CONFIG);
        ADC_Value = adcdata_to_volt(adc_16bit); 
		D =(uint8_t)(ADC_Value*100);
		if(75 <= D && D <78)	
			{
				T =1;
			}	
			else if(78 <= D && D <=80)	
			{
				T =2;
			}
			else if(107 <= D && D <109)	
			{
				T =3;
			}
			else if(109 <= D && D <=111)	
			{
				T =4;
			
			}
			else if(141 <= D && D <143)	
			{
				T =5;
			}
			else if(143 <= D && D <=148)	
			{
				T =6;
			}
			else if(93 <= D && D <=99)	
			{
				T =7;
			}
			else if(100 <= D && D <=103)	
			{
				T =8;
			}
			else if(118 <= D && D <=120)	
			{
				T =9;
			}
			else if(122 <= D && D <=125)	
			{
				T =10;
			}
			else if(140 <= D && D <=142)	
			{
				T =11;
			}
			
        adc_16bit = 0;
        register_data = 0;
        ADC_Value = 0;
		ad3344_Exti_disable();
    }
}



//USART1_PrintFloat4(ADC_Value);
		//usart_printf("  %d  ",T);			
        //ad3344_Exti_enable();  

