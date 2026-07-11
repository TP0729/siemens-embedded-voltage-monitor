#ifndef  _USART_h
#define  _USART_h
#include "gd32f4xx.h"
#include <stdbool.h>

void USART1_Init(void);
void USART1_SendByte(uint8_t data);
void USART1_SendString(const char *str);
bool USART1_DataAvailable(void);
uint8_t USART1_GetChar(void);
void USART1_SendNumber(uint16_t num);
void usart_printf(const char *fmt, ...) ;
void USART1_PrintFloat4(float value);
void print_hex_byte(uint8_t byte);
#endif
	

