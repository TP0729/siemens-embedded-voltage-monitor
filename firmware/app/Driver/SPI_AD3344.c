#include "SPI_AD3344.h"

void ad3344_spi_init(void)
{
	spi_parameter_struct spi_init_struct;
    rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_SPI0);
	
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    

    /* 3. 配置 SPI 引脚：PA5 SCK, PA6 MISO, PA7 MOSI 为复用功能 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);  // SPI0 复用 AF5

    /* 4. 配置 SPI0 参数 */
    
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;      // 16 位数据帧
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;   // 模式1 (CPOL=0, CPHA=1)
    spi_init_struct.nss                  = SPI_NSS_SOFT;             // 软件 NSS
    spi_init_struct.prescale             = SPI_PSC_32;               // 分频 32 (系统时钟/32)
    spi_init_struct.endian               = SPI_ENDIAN_MSB;           // 高位先传
    spi_init(SPI0, &spi_init_struct);

    /* 5. 使能 SPI0 */
    spi_enable(SPI0);
	SPI_SET_CS();  // 空闲时 CS 高电平
}


uint16_t ad3344_spi_txrx16bit(uint16_t tx_byte)
{
    /* 等待发送缓冲区空 */
    while (spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET);
    
    /* 发送数据 */
    spi_i2s_data_transmit(SPI0, tx_byte);
    
    /* 等待接收缓冲区非空 */
    while (spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET);
    
    /* 读取并返回接收到的数据 */
    return (uint16_t)spi_i2s_data_receive(SPI0);
}

uint16_t ADC_Config[2]={0}; 
uint16_t AD3344_CONFIG;

void ad3344_Exti_enable(void)
{
    /* 使能 GPIOA 和 SYSCFG 时钟 */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_SYSCFG);
    
    /* 配置 PA6 为浮空输入模式 */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    
    /* 将 PA6 连接到 EXTI 线 6 */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN6);
    
    /* 配置 EXTI 线 6：下降沿触发中断 */
    exti_init(EXTI_6, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_6);
    
    /* 使能 NVIC 中断（EXTI6 属于 EXTI5_9_IRQn）*/
    nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);
}

void ad3344_Exti_disable(void)
{
    /* 禁用 NVIC 中断 */
    nvic_irq_disable(EXTI5_9_IRQn);
    
    /* 清除并禁用 EXTI 线 6 */
    exti_interrupt_flag_clear(EXTI_6);
    exti_interrupt_disable(EXTI_6);
    
    /* 重新配置 PA6 为 SPI0 的 MISO 复用功能 */
    /* 先使能 SPI0 时钟（如果尚未使能）*/
    rcu_periph_clock_enable(RCU_SPI0);
    
    /* 将 PA6 配置为复用功能模式，AF5（SPI0 MISO）*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_6);
    
    /* 注意：原函数中还会重新配置 PA5 和 PA7，如果你的 SPI 初始化已经做过了，此处可以省略。
       但为了与原代码完全一致，也重新配置 PA5 和 PA7 为 SPI 功能 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_5 | GPIO_PIN_7);
}


uint16_t AD3344_Send_Data(uint16_t config_d)
{
    uint16_t Data;

    Data = ad3344_spi_txrx16bit(config_d);
    
    return (Data);
}

uint16_t ad3344_read_data32(uint16_t config_d, uint16_t *config)
{
    uint16_t data;
    
    data = AD3344_Send_Data(config_d);
    *config = AD3344_Send_Data(0);
    
    return (data);
}

uint16_t ad3344_read_data16(uint16_t config_d)
{
    uint16_t data;
    
    SPI_CLR_CS();
    delay_us(1000);
    
    data = AD3344_Send_Data(config_d);
    
    SPI_SET_CS();
    delay_us(10000);
    
    SPI_CLR_CS();
    
    return (data);
}

uint16_t ad3344_read_regs(uint8_t addr)
{
    uint8_t reg_addr = addr;
    uint16_t reg_rtu = 0;

    SPI_CLR_CS();
    delay_us(1000);
    
    AD3344_Send_Data(reg_addr);
    
    reg_rtu = AD3344_Send_Data(0x00);
    
    SPI_SET_CS();
    delay_us(10000);
    
    SPI_CLR_CS();


    return reg_rtu;
}

void ad3344_process(void)
{
    uint16_t addr,val;
    uint16_t tx_data;
    
    addr = 0x10 + 0x02;
    val = 0xACCA;
    
    SPI_CLR_CS();
    delay_us(1000);
    
    tx_data = 0x8100;
    ad3344_spi_txrx16bit(tx_data);
    
    tx_data = addr;
    ad3344_spi_txrx16bit(tx_data);
    
    tx_data = val;
    ad3344_spi_txrx16bit(tx_data);
    delay_us(1000);
    
    SPI_SET_CS();
    delay_us(1000);
}

void ad3344_ExtRef(void)
{
    uint16_t addr,val,rdval;
    uint16_t tx_data;
    
    addr = 0x10 + 0x4;
    
    SPI_CLR_CS();
    delay_us(1000);
    
    tx_data = 0x8106;
    ad3344_spi_txrx16bit(tx_data);
    delay_us(1000);
    
    tx_data = addr;
    ad3344_spi_txrx16bit(tx_data);
    delay_us(1000);
    
    rdval = ad3344_spi_txrx16bit(0x00);
    delay_us(1000);
    
    SPI_SET_CS();
    delay_us(1000);
    
    val = rdval | 0x40;
    
    SPI_CLR_CS();
    delay_us(1000);
    
    tx_data = 0x8100;
    ad3344_spi_txrx16bit(tx_data);
    
    tx_data = addr;
    ad3344_spi_txrx16bit(tx_data);
    
    tx_data = val;
    ad3344_spi_txrx16bit(tx_data);
    delay_us(1000);
    
    SPI_SET_CS();
    delay_us(1000);
}


void ad3344_init(uint16_t config_d)
{
    SPI_CLR_CS();
    delay_us(1000);
    ad3344_read_data32(config_d, ADC_Config);
    #ifdef BIT32_TRANS_CYCLE
    
    #else
    ad3344_spi_txrx16bit(config_d);
    #endif
    
    delay_us(100);
    
    SPI_SET_CS();
    delay_us(1000);

    SPI_CLR_CS();
    delay_us(1000);
}

void ad3344_stop_conver()
{
    AD3344_CONFIG |= AD3344_REG_CONFIG_MODE_SINGLE;
    
    #ifdef BIT32_TRANS_CYCLE
    ad3344_read_data32(AD3344_CONFIG, ADC_Config);
    #else
    ad3344_spi_txrx16bit(AD3344_CONFIG);
    #endif
}

void ad3344_reset()
{
    #ifdef BIT32_TRANS_CYCLE
    ad3344_read_data32(AD3344_CONFIG_DEFAULT, ADC_Config);
    #else
    ad3344_spi_txrx16bit(AD3344_CONFIG_DEFAULT);
    #endif
}

extern float pga;


void AD3344_reg_Config(uint8_t InputMUX, uint8_t Channel)
{
    if(InputMUX == AD3344_DUAL_END)
    {
        switch (Channel)
        {
        case (0):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_DIFF_0_1;
          break;
        case (1):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_DIFF_0_3;
          break;
        case (2):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_DIFF_1_3;
          break;
        case (3):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_DIFF_2_3;
          break;
        }
    }else if(InputMUX == AD3344_SINGLE_END)
    {
        switch (Channel)
        {
        case (0):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_SINGLE_0;
          break;
        case (1):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_SINGLE_1;
          break;
        case (2):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_SINGLE_2;
          break;
        case (3):
          AD3344_CONFIG |= AD3344_REG_CONFIG_MUX_SINGLE_3;
          break;
        }
    }
    
    AD3344_CONFIG |= AD3344_REG_CONFIG_DR_6_25SPS   ;
    AD3344_CONFIG |= AD3344_REG_CONFIG_PULL_UP_EN;
    AD3344_CONFIG |= AD3344_REG_CONFIG_NOP_VALID;
    
    AD3344_CONFIG |=   AD3344_REG_CONFIG_PGA_2_048V;
    
    if((AD3344_CONFIG&AD3344_REG_CONFIG_PGA_MASK) == AD3344_REG_CONFIG_PGA_6_144V){
        pga = 6.144;
    }else if((AD3344_CONFIG&AD3344_REG_CONFIG_PGA_MASK) == AD3344_REG_CONFIG_PGA_4_096V){
        pga = 4.096;
    }else if((AD3344_CONFIG&AD3344_REG_CONFIG_PGA_MASK) == AD3344_REG_CONFIG_PGA_2_048V){
        pga = 2.048;
    }else if((AD3344_CONFIG&AD3344_REG_CONFIG_PGA_MASK) == AD3344_REG_CONFIG_PGA_1_024V){
        pga = 1.024;
    }else if((AD3344_CONFIG&AD3344_REG_CONFIG_PGA_MASK) == AD3344_REG_CONFIG_PGA_0_512V){
        pga = 0.512;
    }else if((AD3344_CONFIG&AD3344_REG_CONFIG_PGA_MASK) == AD3344_REG_CONFIG_PGA_0_256V){
        pga = 0.256;
    }else{
        pga = 0.064;
    }
    
//    #ifdef CONTINUOUS_CONVERSION
        AD3344_CONFIG |= AD3344_REG_CONFIG_MODE_CONTIN;
  //  #else
      //  AD3344_CONFIG |= AD3344_REG_CONFIG_MODE_SINGLE;
        /* Set 'start single-conversion' bit */
//        AD3344_CONFIG |= AD3344_REG_CONFIG_OS_SINGLE;
//    #endif
	
}























