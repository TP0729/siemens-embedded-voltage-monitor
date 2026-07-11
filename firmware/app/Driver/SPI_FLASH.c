

//////////////////////////////////////////////////////////////////////////////
#include "SPI_FLASH.h"

static uint8_t spi_read_write(uint8_t tx)
{
    while (spi_i2s_flag_get(SPI1, SPI_FLAG_TBE) == RESET);
    spi_i2s_data_transmit(SPI1, tx);
    while (spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE) == RESET);
    return spi_i2s_data_receive(SPI1);
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
    rcu_periph_clock_enable(RCU_SPI1);        // 使用 SPI2
    rcu_periph_clock_enable(RCU_SYSCFG);

    // 禁用 JTAG（释放 PA15, PB3, PB4）
    SYSCFG_CFG0 &= ~(3UL << 8);
    SYSCFG_CFG0 |= (2UL << 8);

    // PB3/4/5 设为浮空输入（避免干扰，因为你的杜邦线是从 PB13/14/15 过来的）
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    // CS 脚 PA15
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    GD25_CS_HIGH();

    // SPI2 引脚：PB13 SCK, PB14 MISO, PB15 MOSI
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);  // SPI1 的 AF 是 5

    // SPI 参数
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;  // 模式0
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_16;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);
    spi_enable(SPI1);
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

#define CONFIG_FLASH_ADDR  0x00010000   // 外部 Flash 地址（64KB 位置）

void save_config_to_flash(config_t *cfg) 
{
    uint8_t *p = (uint8_t *)cfg;
    uint32_t addr = CONFIG_FLASH_ADDR;
    uint32_t i;
    uint32_t size = sizeof(config_t);

    // 擦除扇区（4KB）
    gd25_sector_erase(addr);

    // 分页写入（每页最多 256 字节）
    for (i = 0; i < size; i += 256) {
        uint32_t len = size - i;
        if (len > 256) len = 256;
        gd25_page_program(addr + i, p + i, len);
    }
}


void load_config_from_flash(config_t *cfg)
{
    gd25_read_data(CONFIG_FLASH_ADDR, (uint8_t *)cfg, sizeof(config_t));
}







