#ifndef DAC_H
#define DAC_H
#include "All_HeadFile.h"
#define DAC_SAMPLE_COUNT    200   
extern uint16_t dac_buf[];
#define 	DAC0                              (DAC_BASE)

void DAC_Sin_Data_Gen(uint16_t maxval, uint16_t Frequence, uint16_t *outbuff);
void DAC_GPIO_Config(void);
void DAC_Config(void);
void DMA_Config(void);
void Timer_Config(void);
void DAC_Squre_Wave_Gen(uint16_t val ,uint8_t pwm,uint16_t *outbuff);
void DAC_Tri_Wave_Gen(uint16_t val,uint16_t *outbuff);	
void DAC_Sin_WAVE(void);
void DAC_Init(void);

#endif 


