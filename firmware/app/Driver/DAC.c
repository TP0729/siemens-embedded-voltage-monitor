#include "DAC.h"

// 不再需要波形缓冲区
// uint16_t dac_buf[DAC_SAMPLE_COUNT];

static void DAC_GPIO_Config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
}

static void DAC_Config(void)
{
    rcu_periph_clock_enable(RCU_DAC);
    dac_enable(DAC0, DAC_OUT0);           
    dac_output_buffer_enable(DAC0, DAC_OUT0); 
    
    
    dac_trigger_disable(DAC0, DAC_OUT0);
    dac_dma_disable(DAC0, DAC_OUT0);
    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_DISABLE);
}

void DAC_Set_Output(uint16_t value)
{
    dac_data_set(DAC0, DAC_OUT0, DAC_ALIGN_12B_R, value);
}

void DAC_Init(void)
{   
    DAC_GPIO_Config();
    DAC_Config();
}

uint8_t CH1_F_To_H(float ch1)
{
	uint8_t i;
	i= (uint8_t)(ch1*100);
	i=i/100;
	return i;
}	


//#include "DAC.h"

//uint16_t dac_buf[DAC_SAMPLE_COUNT];


//void DAC_Sin_Data_Gen(uint16_t maxval, uint16_t samples, uint16_t *outbuff)
//{
//    uint16_t i;
//    float outdata = 0;
//    float step = 2 * 3.1415926f / samples;  
//    for (i = 0; i < samples; i++)
//	{
//        outdata = (maxval * (1 + sinf(step * i)))/2;
//        if (outdata > 4095) outdata = 4095;  
//        outbuff[i] = (uint16_t)outdata;       
//    }
//}

//void DAC_Squre_Wave_Gen(uint16_t val ,uint8_t pwm,uint16_t *outbuff)
//{
//	uint16_t i;
//	if (pwm>100) pwm = 100;
//	 for (i = 0; i < 2*pwm; i++)
//	{
//        if (val> 4095) val = 4095;  
//        outbuff[i] = val;       
//    }	
//	for (i = 0; i < 2*(100-pwm); i++)
//	{
//        outbuff[199-i] = 0;       
//    }
//}	

//void DAC_Tri_Wave_Gen(uint16_t val,uint16_t *outbuff)
//{
//	uint16_t i;
//	 for (i = 0; i < 100; i++)
//	{
//        outbuff[i] = (val*i)/100;       
//    }	
//	for (i = 0; i < 100; i++)
//	{
//        outbuff[199-i] = (val*i)/100;       
//    }
//}	



//void DAC_GPIO_Config(void)
//{
//    rcu_periph_clock_enable(RCU_GPIOA);
//    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
//}

//void DAC_Config(void)
//{
//    rcu_periph_clock_enable(RCU_DAC);
//    dac_enable(DAC0, DAC_OUT0);           
//    dac_output_buffer_enable(DAC0, DAC_OUT0); 
//	
//    dac_trigger_enable(DAC0, DAC_OUT0);   
//    dac_trigger_source_config(DAC0, DAC_OUT0, DAC_TRIGGER_T1_TRGO); // 触发源选择 Timer1 的 TRGO
//    dac_dma_enable(DAC0, DAC_OUT0);       
//    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_DISABLE); 
//}

//void DMA_Config(void)
//{
//    dma_single_data_parameter_struct dma_init_struct;

//    rcu_periph_clock_enable(RCU_DMA0);
//    dma_deinit(DMA0, DMA_CH5); 

//    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;          
//    dma_init_struct.memory0_addr = (uint32_t)dac_buf;      
//    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;   
//    dma_init_struct.number = DAC_SAMPLE_COUNT;                
//    dma_init_struct.periph_addr = (uint32_t)(&DAC_OUT0_R12DH(DAC0)); 
//    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  
//    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_16BIT; 
//    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;        
//    dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_ENABLE; 

//    dma_single_data_mode_init(DMA0, DMA_CH5, &dma_init_struct);
//    dma_channel_subperipheral_select(DMA0, DMA_CH5, DMA_SUBPERI7); 
//    dma_channel_enable(DMA0, DMA_CH5);                       
//}

//void Timer_Config(void)
//{
//    timer_parameter_struct timer_initpara;
//	
//    rcu_periph_clock_enable(RCU_TIMER1);
//    timer_deinit(TIMER1);

//    timer_struct_para_init(&timer_initpara);
// 
//    timer_initpara.prescaler = 29;      // 预分频: 120MHz / (29+1) = 4MHz
//    timer_initpara.period = 9;          // 自动重装载值: 4MHz / (9+1) = 400KHz 
//    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
//    timer_initpara.counterdirection = TIMER_COUNTER_UP;
//    timer_init(TIMER1, &timer_initpara);

//    timer_master_output_trigger_source_select(TIMER1, TIMER_TRI_OUT_SRC_UPDATE);    
//    timer_enable(TIMER1); // 使能定时器
//}


//void DAC_Sin_WAVE(void)
//{
//	DAC_Sin_Data_Gen(2047, DAC_SAMPLE_COUNT, dac_buf);
//}	

//void DAC_Init(void)
//{	
//	DAC_GPIO_Config();
//    DAC_Config();
//    DMA_Config();
//    Timer_Config();
//}










//void DAC_Sin_Data_Control_Gen(float maxvol, uint16_t Frequence, uint16_t *outbuff)//FRE<200KHZ
//{
//	timer_parameter_struct timer_initpara;
//    uint16_t i,dig_V,Pr;
//    float outdata = 0;
//    float step = 2 * 3.1415926f / DAC_SAMPLE_COUNT;  
//	if(maxvol > 3.3f  ) maxvol = 3.3;
//	if(maxvol < 0 )   maxvol = 0;
//	dig_V = (uint16_t)((maxvol/3.3f)*4095);
//	
//	Pr = (200000/Frequence)-1;
//    timer_struct_para_init(&timer_initpara);
//    timer_initpara.prescaler = 29;      // 预分频: 120MHz / (29+1) = 4MHz
//    timer_initpara.period = Pr;          // 自动重装载值: 4MHz / (9+1) = 400KHz 
//    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
//    timer_initpara.counterdirection = TIMER_COUNTER_UP;
//    timer_init(TIMER1, &timer_initpara);

//    timer_master_output_trigger_source_select(TIMER1, TIMER_TRI_OUT_SRC_UPDATE);    
//    timer_enable(TIMER1); // 使能定时器
//	
//	
//    for (i = 0; i < DAC_SAMPLE_COUNT; i++)
//	{
//        outdata = (dig_V * (1 + sinf(step * i)))/2;
//        if (outdata > 4095) outdata = 4095;  
//        outbuff[i] = (uint16_t)outdata;       
//    }
//	
//	
//}

