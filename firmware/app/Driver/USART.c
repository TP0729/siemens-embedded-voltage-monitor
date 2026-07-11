#include "USART.h"
#include <stdarg.h>
#include <stdint.h>
static volatile bool rx_ready = false;   // 新数据标志
static volatile uint8_t rx_data = 0;     // 存放接收到的字节
uint32_t B_rate = 19200;


void USART1_Init(void)
{
    // 使能时钟
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);   // 如果你不需要PE8可去掉
    rcu_periph_clock_enable(RCU_USART1);

    // 配置 PD5 (TX) 和 PD6 (RX) 为复用 AF7
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5 | GPIO_PIN_6);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6);
    gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_5 | GPIO_PIN_6);

    // 配置 CS 脚 PE8（可选）
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_bit_reset(GPIOE, GPIO_PIN_8);

    // USART 基本配置
    usart_deinit(USART1);//
	//usart_baudrate_set(USART1, 115200);//
    usart_baudrate_set(USART1, B_rate);//115200
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);

    usart_enable(USART1);
	
    usart_flag_clear(USART1, USART_FLAG_RBNE);
    usart_flag_clear(USART1, USART_FLAG_ORERR);
    usart_flag_clear(USART1, USART_FLAG_FERR);
    while (usart_flag_get(USART1, USART_FLAG_RBNE))
	{
        (void)usart_data_receive(USART1);
	}
    usart_interrupt_enable(USART1, USART_INT_RBNE);
    nvic_irq_enable(USART1_IRQn, 1, 0);
}

// 主动发送字节（阻塞）
void USART1_SendByte(uint8_t data)
{
	gpio_bit_set(GPIOE, GPIO_PIN_8);
    while (usart_flag_get(USART1, USART_FLAG_TBE) == RESET);
    usart_data_transmit(USART1, data);
	//gpio_bit_reset(GPIOE, GPIO_PIN_8);
}

// 主动发送字符串
void USART1_SendString(const char *str)
{
    while (*str)
	{
        USART1_SendByte((uint8_t)*str++);
    }
}

// 检查是否有新数据
bool USART1_DataAvailable(void)
{
    return rx_ready;
}

// 取回最新字节，同时清除新数据标志
uint8_t USART1_GetChar(void)
{
    rx_ready = false;
    return rx_data;
}

void USART1_SendNumber(uint16_t num)
{
    char buf[6];          // 65535 最大 5 位 + 结束符
    char *p = buf + 5;
    *p = '\0';            // 字符串结束符
    do {
        *(--p) = (num % 10) + '0';
        num /= 10;
    } while (num);
    USART1_SendString(p); // 发送数字部分
    USART1_SendString("\r\t");
}



// 发送单个字符（底层，可用你的 USART1_SendByte）
static void usart_putc(char ch)
{
    USART1_SendByte((uint8_t)ch);
}

// 发送字符串
static void usart_puts(const char *str)
{
    while (*str) {
        usart_putc(*str++);
    }
}

// 发送整数（十进制）
static void usart_print_dec(uint32_t val) 
{
    char buf[12];
    char *p = buf + 11;
    *p = '\0';
    do {
        *(--p) = (val % 10) + '0';
        val /= 10;
    } while (val);
    usart_puts(p);
}

// 发送整数（十六进制，小写）
static void usart_print_hex(uint32_t val, uint8_t digits)
{
	int i;
    char buf[9];
    char *p = buf + 8;
    *p = '\0';
    for ( i = 0; i < digits; i++)
	{
        *(--p) = "0123456789abcdef"[val & 0x0F];
        val >>= 4;
    }
    usart_puts(p);
}

// 用户可调用的格式化输出
void usart_printf(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    while (*fmt) 
		{
        if (*fmt == '%')
		{
            fmt++;
            switch (*fmt) {
                case 'd': {
                    int32_t val = va_arg(args, int32_t);
                    if (val < 0) {
                        usart_putc('-');
                        val = -val;
                    }
                    usart_print_dec((uint32_t)val);
                    break;
                }
                case 'u': {
                    uint32_t val = va_arg(args, uint32_t);
                    usart_print_dec(val);
                    break;
                }
                case 'x': {
                    uint32_t val = va_arg(args, uint32_t);
                    usart_print_hex(val, 4);  // 固定输出8位十六进制，可改进
                    break;
                }
                case 's': {
                    const char *str = va_arg(args, const char *);
                    usart_puts(str);
                    break;
                }
                case 'c': {
                    char ch = (char)va_arg(args, int);
                    usart_putc(ch);
                    break;
                }
                case '%': {
                    usart_putc('%');
                    break;
                }
                default:
                    usart_putc('%');
                    usart_putc(*fmt);
                    break;
            }
        } else {
            usart_putc(*fmt);
        }
        fmt++;
    }
    va_end(args);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 打印浮点数，固定小数点后四位
void USART1_PrintFloat4(float value)
{
	int i;
	 uint32_t int_part,frac_part,divisor;
	char buf[12],*p;
    // 处理负数
    if (value < 0) {
        USART1_SendByte('-');
        value = -value;
    }

    // 整数部分
  int_part = (uint32_t)value;
    // 小数部分：乘以 10000 并四舍五入
     frac_part = (uint32_t)((value - int_part) * 10000.0f + 0.5f);

    // 处理进位（例如 3.9999 四舍五入后变成 4.0000）
    if (frac_part >= 10000) {
        int_part++;
        frac_part -= 10000;
    }

    // 打印整数部分（使用已有的数字转字符串打印）
   
	p = buf + 11;
    *p = '\0';
    do {
        *(--p) = (int_part % 10) + '0';
        int_part /= 10;
    } while (int_part);
    USART1_SendString(p);

    // 打印小数点
    USART1_SendByte('.');

    // 打印小数部分（固定4位，不足补零）
     divisor = 1000;
    for ( i = 0; i < 4; i++) {
        USART1_SendByte((frac_part / divisor) + '0');
        frac_part %= divisor;
        divisor /= 10;
    }
}


// USART1 中断服务函数
//void USART1_IRQHandler(void)
//{
//    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE) != RESET) {
//        rx_data = usart_data_receive(USART1);
//        rx_ready = true;
//        usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RBNE);
//    }
//}

void print_hex_byte(uint8_t byte)
{
    uint8_t high = (byte >> 4) & 0x0F;
    uint8_t low  = byte & 0x0F;
    
    /* 打印高位 */
    if (high < 10) usart_printf("%c", '0' + high);
    else usart_printf("%c", 'A' + (high - 10));
    
    /* 打印低位 */
    if (low < 10) usart_printf("%c", '0' + low);
    else usart_printf("%c", 'A' + (low - 10));
}


		
//		 if (USART1_DataAvailable())
//		 {
//			 uint16_t ch = USART1_GetChar();
//			 OLED_ShowNum(16,16,ch,6,16,1);
//			 gpio_bit_toggle(GPIOC,LED_Pin_5);
//			 gpio_bit_set(GPIOE, GPIO_PIN_8);
//			usart_printf("%d度\r\n",ch);
//        }
       

