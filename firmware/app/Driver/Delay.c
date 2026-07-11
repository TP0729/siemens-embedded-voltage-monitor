#include "Delay.h"

/* 使用局部变量 + volatile，防止被优化 */
void delay_us(uint32_t us)
{
    uint32_t i, j;
    for (i = 0; i < us; i++) {
        for (j = 0; j < 46; j++) {
            __NOP();
        }
    }
}

void delay_1ms(uint32_t ms)
{
    uint32_t i;
    for (i = 0; i < ms; i++) {
        delay_us(1000);
    }
}

void delay_s(uint32_t s)
{
    uint32_t i;
    for (i = 0; i < s; i++) {
        delay_1ms(1000);
    }
}
