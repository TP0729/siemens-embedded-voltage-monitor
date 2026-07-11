#ifndef  _ADC_H
#define  _ADC_H
#include "All_HeadFile.h"

void ADC_Init(void);
uint16_t Get_adc_value(void);
float ADC_Volatge(uint16_t ADC_VALUE);
float PC0_Vol_RV(void);
void ADC_OLED_Init(void);
void OLED_ADC_Show_V(void);



uint32_t float_to_hex(float value);
uint32_t float_to_hex_bigendian(float value);
float hex_to_float(uint32_t hex);
#endif
