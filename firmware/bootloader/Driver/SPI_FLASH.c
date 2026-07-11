

//////////////////////////////////////////////////////////////////////////////
#include "SPI_FLASH.h"

static uint8_t spi_read_write(uint8_t tx)
{
    while (spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET);
    spi_i2s_data_transmit(SPI0, tx);
    while (spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET);
    return spi_i2s_data_receive(SPI0);  
}

static void gd25_wait_busy(void)
{
    uint8_t status;
    GD25_CS_LOW();
    spi_read_write(0x05);                    
    do {
        status = spi_read_write(0xFF);
    } while (status & 0x01);                 
    GD25_CS_HIGH();
}

static void gd25_write_enable(void)
{
    GD25_CS_LOW();
    spi_read_write(0x06);
    GD25_CS_HIGH();
}

void GD25_Init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SYSCFG);

    //禁用 JTAG，释放所有 JTAG 引脚PA15 PB3 PB4
    SYSCFG_CFG0 &= ~(3UL << 8);
    SYSCFG_CFG0 |= (2UL << 8);

    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    GD25_CS_HIGH();

   
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

   
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_16;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
    spi_enable(SPI0);
}

uint32_t gd25_read_jedec_id(void)
{
    uint32_t id = 0;
	uint8_t b1,b2,b3,b4; 
    GD25_CS_LOW();
    spi_read_write(0x9F);
   
	 b1 = spi_read_write(0xFF);
     b2 = spi_read_write(0xFF);
     b3 = spi_read_write(0xFF);
     b4 = spi_read_write(0xFF);
    GD25_CS_HIGH();
   
    id = (b1 << 24) | (b2 << 16) | (b3<<8)|b4;
    
    return id;
}

void gd25_read_data(uint32_t addr, uint8_t *buffer, uint32_t len)
{
    uint32_t i;
    GD25_CS_LOW();
    spi_read_write(0x03);
    spi_read_write((addr >> 16) & 0xFF);
    spi_read_write((addr >> 8) & 0xFF);
    spi_read_write(addr & 0xFF);
    for (i = 0; i < len; i++) {
        buffer[i] = spi_read_write(0xFF);
    }
    GD25_CS_HIGH();
}


void gd25_page_program(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t i;
    if (len == 0 || len > 256) return;
    
    gd25_write_enable();                         
    
    GD25_CS_LOW();
    spi_read_write(0x02);
    spi_read_write((addr >> 16) & 0xFF);
    spi_read_write((addr >> 8) & 0xFF);
    spi_read_write(addr & 0xFF);
    for (i = 0; i < len; i++) {
        spi_read_write(data[i]);
    }
    GD25_CS_HIGH();
    
    gd25_wait_busy();
}

void gd25_sector_erase(uint32_t addr)
{
    gd25_write_enable();                         
    
    GD25_CS_LOW();
    spi_read_write(0x20);
    spi_read_write((addr >> 16) & 0xFF);
    spi_read_write((addr >> 8) & 0xFF);
    spi_read_write(addr & 0xFF);
    GD25_CS_HIGH();
    
    gd25_wait_busy();
}

uint8_t gd25_read_device_id(void)
{
    uint8_t id;
    GD25_CS_LOW();
    spi_read_write(0xAB);
    spi_read_write(0xFF);
    spi_read_write(0xFF);
    spi_read_write(0xFF);
    id = spi_read_write(0xFF);
    GD25_CS_HIGH();
    return id;
}
