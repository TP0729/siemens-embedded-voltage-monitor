#include "Protocol.h"


//usart_printf("A5B6%xFFEEEE0002E087B6A5",ID);数据错误
// 十六进制字符转数值
static uint8_t hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

// 两个ASCII字符转一个字节
static uint8_t hex_to_byte(char high, char low) {
    return (hex_val(high) << 4) | hex_val(low);
}

// CRC-16-Modbus 计算
static uint16_t crc16_modbus(const uint8_t *data, uint16_t len)
{	uint16_t i;
	int j;
    uint16_t crc = 0xFFFF;
    for ( i = 0; i < len; i++) {
        crc ^= data[i];
        for ( j = 0; j < 8; j++) {
            if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}


// 解析一帧，成功返回1，失败返回0
int parse_received_frame(parsed_frame_t *out)
{
    uint16_t i, j;
    uint16_t crc_calc, crc_rcv;
    uint16_t consumed;
    uint8_t bin[256];
    uint16_t bin_len;
    uint8_t bin_tmp[9];
    uint8_t data_len;
    uint16_t expected_bytes, expected_chars;
    uint8_t is_error;
    uint16_t err_device_id;
    
    for (i = 0; i + 4 <= rx_len; i++) {
        if (rx_buffer[i] == 'A' && rx_buffer[i+1] == '5' &&
            rx_buffer[i+2] == 'B' && rx_buffer[i+3] == '6') {
            
            if (i + 18 > rx_len) continue;
            
            for (j = 0; j < 9; j++) {
                bin_tmp[j] = hex_to_byte(rx_buffer[i + j*2], rx_buffer[i + j*2 + 1]);
            }
            data_len = bin_tmp[7];
            expected_bytes = 2 + 2 + 1 + 2 + 1 + 1 + data_len + 2 + 2;
            expected_chars = expected_bytes * 2;
            
            if (i + expected_chars > rx_len) continue;
            
            bin_len = 0;
            for (j = i; j < i + expected_chars; j += 2) {
                bin[bin_len++] = hex_to_byte(rx_buffer[j], rx_buffer[j+1]);
            }
            
            is_error = 0;
            
            if (bin[bin_len-2] != 0xB6 || bin[bin_len-1] != 0xA5) {
                is_error = 1;
            }
            
            crc_calc = crc16_modbus(bin, expected_bytes - 4);
            crc_rcv = (bin[expected_bytes-4] << 8) | bin[expected_bytes-3];
            if (crc_calc != crc_rcv) {
                is_error = 1;
            }
            
            err_device_id = (bin[2] << 8) | bin[3];
            
            consumed = expected_chars;
            memmove(rx_buffer + i, rx_buffer + i + consumed, rx_len - i - consumed);
            rx_len -= consumed;
            
            if (is_error) {
                out->error = 1;
                out->device_id = err_device_id;
                out->frame_type = bin[4];
                out->command = (bin[5] << 8) | bin[6];
                out->data_len = data_len;
                if (out->data_len > 0 && out->data_len <= 250) {
                    memcpy(out->data, &bin[9], out->data_len);
                }
                return 1;
            }
            
            out->error = 0;
            out->device_id = (bin[2] << 8) | bin[3];
            out->frame_type = bin[4];
            out->command = (bin[5] << 8) | bin[6];
            out->data_len = data_len;
            if (out->data_len > 0 && out->data_len <= 250) {
                memcpy(out->data, &bin[9], out->data_len);
            }
            return 1;
        }
    }
    return 0;
}
//int parse_received_frame(parsed_frame_t *out)
//{		
//		uint16_t crc_calc,crc_rcv,consumed;
//		uint16_t i,j;
//		uint8_t bin[256];
//        uint16_t bin_len = 0;
//		for (i = 0; i + 4 <= rx_len; i++)
//		{
//        // 查找起始标志 "A5B6"
//        if (rx_buffer[i] == 'A' && rx_buffer[i+1] == '5' &&rx_buffer[i+2] == 'B' && rx_buffer[i+3] == '6') 
//			{
//            
//            // 至少需要18个字符才能解析前9个字节
//            if (i + 18 > rx_len) continue;
//            
//            // 临时二进制缓冲区
//           
//            // 解码从起始位置开始的尽可能多的字符
//            for ( j = i; j + 1 <= rx_len; j += 2) {
//                bin[bin_len++] = hex_to_byte(rx_buffer[j], rx_buffer[j+1]);
//                if (bin_len >= 256) break;
//            }
//            
//            // 检查最小长度（至少包含结束标志）
//            if (bin_len < 13) continue;
//            
//            // 检查结束标志
//            if (bin[bin_len-2] != 0xB6 || bin[bin_len-1] != 0xA5) continue;
//            
//            // 计算CRC（不含CRC字段本身和结束标志）
//             crc_calc = crc16_modbus(bin, bin_len - 4);
//             crc_rcv = (bin[bin_len-4] << 8) | bin[bin_len-3];
//            if (crc_calc != crc_rcv) continue;
//            
//            // 提取字段
//            out->device_id  = (bin[2] << 8) | bin[3];
//            out->frame_type = bin[4];
//            out->command    = (bin[5] << 8) | bin[6];
//            out->data_len   = bin[7];
//            if (out->data_len > 0 && out->data_len <= 250) {
//                memcpy(out->data, &bin[9], out->data_len);
//            }
//            
//            // 从接收缓冲区中移除已处理的ASCII字符
//           consumed = bin_len * 2;
//            if (consumed < rx_len) {
//                memmove(rx_buffer, rx_buffer + consumed, rx_len - consumed);
//                rx_len -= consumed;
//            } else {
//                rx_len = 0;
//            }
//			
//            return 1;
//        }
//    }
//    return 0;
//}

static uint8_t RE_RES;

uint8_t External_ADC_Sample_T(uint8_t T,uint32_t id)
{
	if(T == 1)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 C2451630 5A67 B6A5",id);
			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}
		if(T == 2)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 C231F76E E8EE B6A5",id);
			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}	
		if(T == 3)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 4205C28F FE51 B6A5",id);
			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}	
		if(T == 4)
		{
			T =0;
			if(RE_RES==0)
			{
				usart_printf("A5B6 %x 02 0221 04 02 421CE100 ADD8 B6A5",id);	
				RE_RES=1;
				delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
				
			}
			else  if(RE_RES==1)
			{					 
				usart_printf("A5B6 %x 02 0221 04 02 421A7400 FC57 B6A5",id);
				RE_RES=0;
				delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
				
			}
//			delay_1ms(1);
//			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}	
		
		if(T == 5)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 43027333 2294 B6A5",id);

			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}	
		if(T == 6)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 430D1C29 1A08 B6A5",id);

			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}
		if(T == 7)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 00000000 4345 B6A5",id);

			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}
		if(T == 8)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 41905600 F26E B6A5",id);

			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}
		if(T == 9)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 42809F71 07F9 B6A5",id);

			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}
		if(T == 10)
		{
			T =0;
			usart_printf("A5B6 %x 02 0221 04 02 429B5C29 0AD8 B6A5",id);

			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}
		if(T == 11)
		{
			T =0;
			usart_printf("A5B6%x 02 0221 04 02 42CF999A 3F8B B6A5",id);

			delay_1ms(1);
			gpio_bit_reset(GPIOE, GPIO_PIN_8);
		}
		return	0;

}	




// 串口中断接收（假设USART1）
void USART1_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE) != RESET) {
        char ch = (char)usart_data_receive(USART1);
        if (rx_len < RX_BUF_SIZE - 1) {
            rx_buffer[rx_len++] = ch;
            rx_buffer[rx_len] = '\0';
        }
        usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RBNE);
    }
}



uint16_t CRC16_modbus(const uint8_t *data, uint16_t len)
{
	uint16_t i,j;
    uint16_t crc = 0xFFFF;
    for ( i = 0; i < len; i++) {
        crc ^= data[i];
        for ( j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}



