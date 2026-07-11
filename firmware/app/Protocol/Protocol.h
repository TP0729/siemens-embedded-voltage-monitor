#ifndef  _PROTOCOL_H
#define  _PROTOCOL_H
#include "gd32f4xx.h"
#include <string.h>
#include <stdio.h>
#include "USART.h"
#include "Delay.h"
// 接收缓冲区
#define RX_BUF_SIZE     512
static char rx_buffer[RX_BUF_SIZE];
static uint16_t rx_len = 0;
typedef struct {
    uint16_t device_id;
    uint8_t  frame_type;
    uint16_t command;
    uint8_t  data_len;
    uint8_t  data[256];
    uint8_t  error;          // 0: 正常帧，1: 错误帧（CRC/长度/类型）
} parsed_frame_t;

int parse_received_frame(parsed_frame_t *out); 

uint8_t External_ADC_Sample_T(uint8_t T,uint32_t id);

uint16_t CRC16_modbus(const uint8_t *data, uint16_t len);




#endif









































