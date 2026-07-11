#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "gd32f4xx.h"
#include <stdio.h>
#include "Delay.h"
#define DUMMY                          0xFF

#define GD25_WRITE_ENABLE              0x06
#define GD25_WRITE_DISABLE             0x04
#define GD25_READ_STATUS_REG           0x05
#define GD25_WRITE_STATUS_REG          0x01
#define GD25_READ_DATA                 0x03
#define GD25_FAST_READ_DATA            0x0B
#define GD25_PAGE_PROGRAM              0x02
#define GD25_SECTOR_ERASE              0x20
#define GD25_BLOCK_ERASE               0xD8
#define GD25_CHIP_ERASE                0xC7
#define GD25_POWER_DOWN                0xB9
#define GD25_RELEASE_POWER_DOWN        0xAB
#define GD25_DEVICE_ID                 0xAB
#define GD25_MANUFACT_DEVICE_ID        0x90
#define GD25_JEDEC_ID                  0x9F

#define GD25_PAGE_SIZE                 256
#define GD25_SECTOR_SIZE               4096
#define GD25_TOTAL_SECTORS             128

#define GD25_CS_LOW()                  gpio_bit_reset(GPIOA, GPIO_PIN_15)
#define GD25_CS_HIGH()                 gpio_bit_set(GPIOA, GPIO_PIN_15)

#define SYSCFGBASE                    (0x40013800UL)
#define SYSCFGCFG0                    (*(volatile uint32_t *)(SYSCFG_BASE + 0x00U))

void  GD25_Init(void);
uint32_t gd25_read_jedec_id(void);

void gd25_read_data(uint32_t addr, uint8_t *buffer, uint32_t len);
void gd25_page_program(uint32_t addr, const uint8_t *data, uint32_t len);
void gd25_sector_erase(uint32_t addr);
void gd25_chip_erase(void);

#endif
