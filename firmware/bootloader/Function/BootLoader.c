#include "BootLoader.h"

int i;
void jump_to_app(void)
{
    uint32_t app_stack = *(uint32_t *)0x08011000;          
    uint32_t app_reset = *(uint32_t *)(0x08011000 + 4);   

    if (app_stack < 0x20000000 || app_stack > 0x20020000) {
        while (1);   
    }

    __disable_irq();               // 关闭全局中断
    SysTick->CTRL = 0;             // 关闭 SysTick 中断
    __set_MSP(app_stack);          // 设置主堆栈指针
    SCB->VTOR = 0x08011000;        // 设置中断向量表偏移

    ((void (*)(void))app_reset)(); // 跳转到 APP 复位向量
}

#define APP_START_ADDR 0x08011000

void JumpToApp(void)
{
    uint32_t jumpAddr = *(volatile uint32_t*)(APP_START_ADDR + 4);
    void (*app_main)(void) = (void (*)(void))jumpAddr;

    // 1. 设置APP的栈指针 (MSP)
    __set_MSP(*(volatile uint32_t*)APP_START_ADDR);
    // 2. 清除所有挂起的中断
    for ( i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    // 3. 执行跳转
    app_main();
}

