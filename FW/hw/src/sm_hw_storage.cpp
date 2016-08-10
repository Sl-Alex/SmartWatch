#include "sm_hw_storage.h"
#include "sm_crc.h"

#ifndef PC_SOFTWARE
#include "sm_hal_gpio.h"
#include "sm_hal_spi_hw.h"

#define MOSI_PORT_BASE  GPIOB_BASE
#define MOSI_PIN        15
#define MISO_PORT_BASE  GPIOB_BASE
#define MISO_PIN        14
#define SCK_PORT_BASE   GPIOB_BASE
#define SCK_PIN         13
#define SS_PORT_BASE    GPIOB_BASE
#define SS_PIN          12

#define SPI_BASE        SPI2_BASE



#define FLASH_DUMMY_BYTE                0xA5

#define FLASH_CMD_WRITE_ENABLE          0x06
#define FLASH_CMD_WRITE_DISABLE         0x04
#define FLASH_CMD_READ_IDENTIFICATION   0x9F
#define FLASH_CMD_READ_STATUS_REGISTER  0x05
#define FLASH_CMD_WRITE_STATUS_REGISTER 0x01
#define FLASH_CMD_READ_DATA_BYTES       0x03
#define FLASH_CMD_READ_DATA_BYTES_HS    0x0B
#define FLASH_CMD_PAGE_PROGRAM          0x02
#define FLASH_CMD_SECTOR_ERASE          0xD8
#define FLASH_CMD_BULK_ERASE            0xC7
#define FLASH_CMD_POWERDOWN             0xB9
#define FLASH_CMD_WAKE                  0xAB

#define FLASH_SPI_PAGESIZE              0x100

/*!< Write In Progress (WIP) flag */
#define FLASH_FLAG_WIP                  0x01

unsigned char out_bin[] = {
  0x09, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00,
  0xdc, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x68, 0x01, 0x00, 0x00,
  0x8c, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00,
  0x80, 0x02, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00,
  0x8c, 0x00, 0x00, 0x00, 0x98, 0x03, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00,
  0x24, 0x04, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0xb0, 0x04, 0x00, 0x00,
  0x24, 0x05, 0x00, 0x00, 0xc8, 0xc8, 0x55, 0x12, 0x20, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x40, 0x20, 0xd0, 0x18, 0x2c, 0x44, 0x3c, 0x04, 0x06, 0x3b, 0x5d, 0x2e,
  0x18, 0xd8, 0xec, 0x74, 0xb8, 0x60, 0xa0, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x90, 0x9b, 0x9c, 0x09, 0x01, 0x00, 0x80,
  0x40, 0x20, 0x90, 0xc8, 0xe8, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x0d,
  0x96, 0x93, 0x93, 0x52, 0x7a, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xc0, 0x20, 0xac, 0xb2, 0x39, 0x28, 0x30, 0x23, 0x24, 0x24, 0x27, 0x23,
  0x21, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x31, 0x32, 0x1a, 0xce, 0x47,
  0xc4, 0x04, 0x04, 0xc4, 0x44, 0x48, 0x50, 0x60, 0x03, 0x06, 0x0f, 0x0e,
  0x0d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x10, 0x27, 0x44, 0xc7, 0xc0, 0xc0, 0xc7,
  0xc4, 0x64, 0x34, 0x1c, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0xd0, 0x18,
  0x2c, 0x44, 0x3c, 0x04, 0x06, 0x3b, 0x5d, 0x2e, 0x18, 0xd8, 0xec, 0x74,
  0xb8, 0x60, 0xa0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x60, 0x90, 0x9b, 0x9c, 0x09, 0x01, 0x00, 0x80, 0x40, 0x20, 0x90, 0xc8,
  0xe8, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x0d, 0x96, 0x93, 0x93, 0x52,
  0x7a, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0xac, 0xb2,
  0x39, 0x28, 0x30, 0x23, 0x24, 0x24, 0x27, 0x23, 0x21, 0x20, 0x20, 0x20,
  0x20, 0x30, 0x30, 0x31, 0x32, 0x1a, 0xce, 0x47, 0xc4, 0x04, 0x04, 0xc4,
  0x44, 0x48, 0x50, 0x60, 0x03, 0x06, 0x0f, 0x0e, 0x0d, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x10, 0x27, 0x44, 0xc7, 0xc0, 0xc0, 0xc7, 0xc4, 0x64, 0x34, 0x1c,
  0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0xd0, 0x18, 0x2c, 0x44, 0x3c, 0x04,
  0x06, 0x3b, 0x5d, 0x2e, 0x18, 0xd8, 0xec, 0x74, 0xb8, 0x60, 0xa0, 0xc0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x90, 0x9b, 0x9c,
  0x09, 0x01, 0x00, 0x80, 0x40, 0x20, 0x90, 0xc8, 0xe8, 0xf8, 0x70, 0x00,
  0x00, 0x00, 0x01, 0x0d, 0x96, 0x93, 0x93, 0x52, 0x7a, 0x0c, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0xac, 0xb2, 0x39, 0x28, 0x30, 0x23,
  0x24, 0x24, 0x27, 0x23, 0x21, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x31,
  0x32, 0x1a, 0xce, 0x47, 0xc4, 0x04, 0x04, 0xc4, 0x44, 0x48, 0x50, 0x60,
  0x03, 0x06, 0x0f, 0x0e, 0x0d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x10, 0x27, 0x44,
  0xc7, 0xc0, 0xc0, 0xc7, 0xc4, 0x64, 0x34, 0x1c, 0x20, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x40, 0x20, 0xd0, 0x18, 0x2c, 0x44, 0x3c, 0x04, 0x06, 0x3b, 0x5d, 0x2e,
  0x18, 0xd8, 0xec, 0x74, 0xb8, 0x60, 0xa0, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x90, 0x9b, 0x9c, 0x09, 0x01, 0x00, 0x80,
  0x40, 0x20, 0x90, 0xc8, 0xe8, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x0d,
  0x96, 0x93, 0x93, 0x52, 0x7a, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xc0, 0x20, 0xac, 0xb2, 0x39, 0x28, 0x30, 0x23, 0x24, 0x24, 0x27, 0x23,
  0x21, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x31, 0x32, 0x1a, 0xce, 0x47,
  0xc4, 0x04, 0x04, 0xc4, 0x44, 0x48, 0x50, 0x60, 0x03, 0x06, 0x0f, 0x0e,
  0x0d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x10, 0x27, 0x44, 0xc7, 0xc0, 0xc0, 0xc7,
  0xc4, 0x64, 0x34, 0x1c, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0xd0, 0x18,
  0x2c, 0x44, 0x3c, 0x04, 0x06, 0x3b, 0x5d, 0x2e, 0x18, 0xd8, 0xec, 0x74,
  0xb8, 0x60, 0xa0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x60, 0x90, 0x9b, 0x9c, 0x09, 0x01, 0x00, 0x80, 0x40, 0x20, 0x90, 0xc8,
  0xe8, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x0d, 0x96, 0x93, 0x93, 0x52,
  0x7a, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0xac, 0xb2,
  0x39, 0x28, 0x30, 0x23, 0x24, 0x24, 0x27, 0x23, 0x21, 0x20, 0x20, 0x20,
  0x20, 0x30, 0x30, 0x31, 0x32, 0x1a, 0xce, 0x47, 0xc4, 0x04, 0x04, 0xc4,
  0x44, 0x48, 0x50, 0x60, 0x03, 0x06, 0x0f, 0x0e, 0x0d, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x10, 0x27, 0x44, 0xc7, 0xc0, 0xc0, 0xc7, 0xc4, 0x64, 0x34, 0x1c,
  0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0xd0, 0x18, 0x2c, 0x44, 0x3c, 0x04,
  0x06, 0x3b, 0x5d, 0x2e, 0x18, 0xd8, 0xec, 0x74, 0xb8, 0x60, 0xa0, 0xc0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x90, 0x9b, 0x9c,
  0x09, 0x01, 0x00, 0x80, 0x40, 0x20, 0x90, 0xc8, 0xe8, 0xf8, 0x70, 0x00,
  0x00, 0x00, 0x01, 0x0d, 0x96, 0x93, 0x93, 0x52, 0x7a, 0x0c, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0xac, 0xb2, 0x39, 0x28, 0x30, 0x23,
  0x24, 0x24, 0x27, 0x23, 0x21, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x31,
  0x32, 0x1a, 0xce, 0x47, 0xc4, 0x04, 0x04, 0xc4, 0x44, 0x48, 0x50, 0x60,
  0x03, 0x06, 0x0f, 0x0e, 0x0d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x10, 0x27, 0x44,
  0xc7, 0xc0, 0xc0, 0xc7, 0xc4, 0x64, 0x34, 0x1c, 0x20, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x40, 0x20, 0xd0, 0x18, 0x2c, 0x44, 0x3c, 0x04, 0x06, 0x3b, 0x5d, 0x2e,
  0x18, 0xd8, 0xec, 0x74, 0xb8, 0x60, 0xa0, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x60, 0x90, 0x9b, 0x9c, 0x09, 0x01, 0x00, 0x80,
  0x40, 0x20, 0x90, 0xc8, 0xe8, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x0d,
  0x96, 0x93, 0x93, 0x52, 0x7a, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xc0, 0x20, 0xac, 0xb2, 0x39, 0x28, 0x30, 0x23, 0x24, 0x24, 0x27, 0x23,
  0x21, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x31, 0x32, 0x1a, 0xce, 0x47,
  0xc4, 0x04, 0x04, 0xc4, 0x44, 0x48, 0x50, 0x60, 0x03, 0x06, 0x0f, 0x0e,
  0x0d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x10, 0x27, 0x44, 0xc7, 0xc0, 0xc0, 0xc7,
  0xc4, 0x64, 0x34, 0x1c, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0xd0, 0x18,
  0x2c, 0x44, 0x3c, 0x04, 0x06, 0x3b, 0x5d, 0x2e, 0x18, 0xd8, 0xec, 0x74,
  0xb8, 0x60, 0xa0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x60, 0x90, 0x9b, 0x9c, 0x09, 0x01, 0x00, 0x80, 0x40, 0x20, 0x90, 0xc8,
  0xe8, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x0d, 0x96, 0x93, 0x93, 0x52,
  0x7a, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0xac, 0xb2,
  0x39, 0x28, 0x30, 0x23, 0x24, 0x24, 0x27, 0x23, 0x21, 0x20, 0x20, 0x20,
  0x20, 0x30, 0x30, 0x31, 0x32, 0x1a, 0xce, 0x47, 0xc4, 0x04, 0x04, 0xc4,
  0x44, 0x48, 0x50, 0x60, 0x03, 0x06, 0x0f, 0x0e, 0x0d, 0x0c, 0x0c, 0x0c,
  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
  0x0c, 0x10, 0x27, 0x44, 0xc7, 0xc0, 0xc0, 0xc7, 0xc4, 0x64, 0x34, 0x1c,
  0x3a, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x68, 0x01, 0x00, 0x00, 0x75, 0x01, 0x00, 0x00, 0x85, 0x01, 0x00, 0x00,
  0x96, 0x01, 0x00, 0x00, 0xa7, 0x01, 0x00, 0x00, 0xb8, 0x01, 0x00, 0x00,
  0xc9, 0x01, 0x00, 0x00, 0xd7, 0x01, 0x00, 0x00, 0xe6, 0x01, 0x00, 0x00,
  0xf5, 0x01, 0x00, 0x00, 0x06, 0x02, 0x00, 0x00, 0x17, 0x02, 0x00, 0x00,
  0x25, 0x02, 0x00, 0x00, 0x36, 0x02, 0x00, 0x00, 0x44, 0x02, 0x00, 0x00,
  0x55, 0x02, 0x00, 0x00, 0x66, 0x02, 0x00, 0x00, 0x75, 0x02, 0x00, 0x00,
  0x86, 0x02, 0x00, 0x00, 0x97, 0x02, 0x00, 0x00, 0xa8, 0x02, 0x00, 0x00,
  0xb9, 0x02, 0x00, 0x00, 0xca, 0x02, 0x00, 0x00, 0xdb, 0x02, 0x00, 0x00,
  0xec, 0x02, 0x00, 0x00, 0xfd, 0x02, 0x00, 0x00, 0x0b, 0x03, 0x00, 0x00,
  0x19, 0x03, 0x00, 0x00, 0x29, 0x03, 0x00, 0x00, 0x3a, 0x03, 0x00, 0x00,
  0x4a, 0x03, 0x00, 0x00, 0x5b, 0x03, 0x00, 0x00, 0x6c, 0x03, 0x00, 0x00,
  0x7d, 0x03, 0x00, 0x00, 0x8e, 0x03, 0x00, 0x00, 0x9f, 0x03, 0x00, 0x00,
  0xb0, 0x03, 0x00, 0x00, 0xc1, 0x03, 0x00, 0x00, 0xd2, 0x03, 0x00, 0x00,
  0xe3, 0x03, 0x00, 0x00, 0xf4, 0x03, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00,
  0x14, 0x04, 0x00, 0x00, 0x25, 0x04, 0x00, 0x00, 0x36, 0x04, 0x00, 0x00,
  0x47, 0x04, 0x00, 0x00, 0x58, 0x04, 0x00, 0x00, 0x69, 0x04, 0x00, 0x00,
  0x7a, 0x04, 0x00, 0x00, 0x8b, 0x04, 0x00, 0x00, 0x9c, 0x04, 0x00, 0x00,
  0xad, 0x04, 0x00, 0x00, 0xbe, 0x04, 0x00, 0x00, 0xcf, 0x04, 0x00, 0x00,
  0xe0, 0x04, 0x00, 0x00, 0xf1, 0x04, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00,
  0x13, 0x05, 0x00, 0x00, 0x21, 0x00, 0x22, 0x00, 0x23, 0x00, 0x24, 0x00,
  0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2a, 0x00,
  0x2b, 0x00, 0x2c, 0x00, 0x2d, 0x00, 0x2e, 0x00, 0x2f, 0x00, 0x30, 0x00,
  0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00,
  0x37, 0x00, 0x38, 0x00, 0x39, 0x00, 0x3a, 0x00, 0x3b, 0x00, 0x3c, 0x00,
  0x3d, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0x40, 0x00, 0x41, 0x00, 0x42, 0x00,
  0x43, 0x00, 0x44, 0x00, 0x45, 0x00, 0x46, 0x00, 0x47, 0x00, 0x48, 0x00,
  0x49, 0x00, 0x4a, 0x00, 0x4b, 0x00, 0x4c, 0x00, 0x4d, 0x00, 0x4e, 0x00,
  0x4f, 0x00, 0x50, 0x00, 0x51, 0x00, 0x52, 0x00, 0x53, 0x00, 0x54, 0x00,
  0x55, 0x00, 0x56, 0x00, 0x57, 0x00, 0x58, 0x00, 0x59, 0x00, 0x5a, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x5f, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
  0x00, 0x07, 0x00, 0x00, 0x07, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
  0x00, 0x05, 0x00, 0x00, 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, 0x05, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x24, 0x2a,
  0x7f, 0x2a, 0x12, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x23, 0x13, 0x08, 0x64, 0x62, 0x05, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x36, 0x49, 0x55, 0x22,
  0x50, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
  0x00, 0x05, 0x03, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03,
  0x00, 0x00, 0x00, 0x1c, 0x22, 0x41, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x41, 0x22, 0x1c, 0x05, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c,
  0x2a, 0x08, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00,
  0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x02, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0xe0, 0x05, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x30, 0x30, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x05, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3e, 0x51, 0x49,
  0x45, 0x3e, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x42, 0x7f, 0x40, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
  0x00, 0x05, 0x00, 0x00, 0x00, 0x62, 0x51, 0x49, 0x49, 0x46, 0x05, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x22, 0x41,
  0x49, 0x49, 0x36, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x18, 0x14, 0x12, 0x7f, 0x10, 0x05, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x27, 0x45, 0x45, 0x45,
  0x39, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
  0x00, 0x3c, 0x4a, 0x49, 0x49, 0x30, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x71, 0x09, 0x05, 0x03, 0x05,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x36,
  0x49, 0x49, 0x49, 0x36, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x06, 0x49, 0x49, 0x29, 0x1e, 0x02, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x36, 0x36, 0x02,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x2c,
  0xec, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
  0x00, 0x08, 0x14, 0x22, 0x41, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
  0x00, 0x05, 0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x04, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x41, 0x22,
  0x14, 0x08, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00,
  0x00, 0x00, 0x02, 0x01, 0x51, 0x09, 0x06, 0x05, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x32, 0x49, 0x79, 0x41, 0x3e,
  0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x7e, 0x09, 0x09, 0x09, 0x7e, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
  0x00, 0x05, 0x00, 0x00, 0x00, 0x7f, 0x49, 0x49, 0x49, 0x36, 0x05, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3e, 0x41,
  0x41, 0x41, 0x22, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x7f, 0x41, 0x41, 0x22, 0x1c, 0x05, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x7f, 0x49, 0x49, 0x49,
  0x41, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
  0x00, 0x7f, 0x09, 0x09, 0x09, 0x01, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3e, 0x41, 0x41, 0x51, 0x72, 0x05,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x7f,
  0x08, 0x08, 0x08, 0x7f, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x41, 0x7f, 0x41, 0x05, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x20, 0x40, 0x41, 0x3f, 0x01,
  0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x7f, 0x08, 0x14, 0x22, 0x41, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
  0x00, 0x05, 0x00, 0x00, 0x00, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x05, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x7f, 0x02,
  0x0c, 0x02, 0x7f, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x7f, 0x04, 0x08, 0x10, 0x7f, 0x05, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3e, 0x41, 0x41, 0x41,
  0x3e, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
  0x00, 0x7f, 0x09, 0x09, 0x09, 0x06, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3e, 0x41, 0x51, 0x21, 0x5e, 0x05,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x7f,
  0x09, 0x19, 0x29, 0x46, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x26, 0x49, 0x49, 0x49, 0x32, 0x05, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x7f,
  0x01, 0x01, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00,
  0x00, 0x00, 0x3f, 0x40, 0x40, 0x40, 0x3f, 0x05, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x1f, 0x20, 0x40, 0x20, 0x1f,
  0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x3f, 0x40, 0x38, 0x40, 0x3f, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
  0x00, 0x05, 0x00, 0x00, 0x00, 0x63, 0x14, 0x08, 0x14, 0x63, 0x05, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x07, 0x08,
  0x70, 0x08, 0x07, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x61, 0x51, 0x49, 0x45, 0x43
};
unsigned int out_bin_len = 2516;

void SmHwStorage::init(void)
{
    mSpi = new SmHalSpiHw<SPI_BASE, SM_HAL_SPI_CFG_FULL_DUPLEX>();

    SmHalGpio<MOSI_PORT_BASE, MOSI_PIN> spiMosi;
    SmHalGpio<MISO_PORT_BASE, MISO_PIN> spiMiso;
    SmHalGpio<SCK_PORT_BASE,  SCK_PIN>  spiSck;

    spiMosi.setModeSpeed(SM_HAL_GPIO_MODE_AF_PP,SM_HAL_GPIO_SPEED_50M);
    spiMiso.setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT,SM_HAL_GPIO_SPEED_50M);
    spiSck.setModeSpeed(SM_HAL_GPIO_MODE_AF_PP,SM_HAL_GPIO_SPEED_50M);

    mSpi->setSsPin(new SmHalGpio<SS_PORT_BASE, SS_PIN>());
    ((SmHalSpiHw<SPI_BASE, SM_HAL_SPI_CFG_FULL_DUPLEX> *)mSpi)->init(SM_HAL_SPI_MODE3, SM_HAL_SPI_WIDTH_8);

    SmHwPowerMgr::getInstance()->subscribe(this);

    readId();

//    eraseBulk();
//    writeBuffer(out_bin, 0, out_bin_len);

    // Read total number of elements in the storage
    readData(0, (uint8_t *)&mCount, sizeof(mCount));

    // Verify CRC
    uint32_t crc32 = SmCrc::calc32(0xFFFFFFFF, (uint8_t *)&mCount, sizeof(mCount));
    uint32_t addr = sizeof(mCount);
    SmHwStorageElementInfo elementInfo;
    // Next element offset. We can check if it is correct
    uint32_t nextOffset = sizeof(mCount) + sizeof(elementInfo)*mCount + sizeof(crc32);
    for (uint32_t i = 0; i < mCount; ++i)
    {
        // Read element info
        readData(addr, (uint8_t *)&elementInfo, sizeof(elementInfo));
        crc32 = SmCrc::calc32(crc32, (uint8_t *)&elementInfo, sizeof(elementInfo));
        addr += sizeof(elementInfo);

        if(elementInfo.offset != nextOffset)
        {
            mCount = 0;
            return;
        }

        // Calculate next element offset
        nextOffset += elementInfo.size;
    }
    uint32_t crcStored;
    readData(addr, (uint8_t *)&crcStored, sizeof(crcStored));
    if (crcStored != crc32)
        mCount = 0;
}

uint32_t SmHwStorage::getElementSize(uint8_t element)
{
    SmHwStorageElementInfo elementInfo;

    if (getElementInfo(element, &elementInfo) == false)
        return 0;

    return elementInfo.size;
}

bool SmHwStorage::getElementInfo(uint8_t element, SmHwStorageElementInfo * info)
{
    if (element >= mCount)
        return false;

    // Read element info
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)info, sizeof(SmHwStorageElementInfo));

    return true;
}

void SmHwStorage::readElement(uint8_t element, uint32_t offset, uint8_t * pData, uint32_t size)
{
    if (element >= mCount)
        return;

    uint32_t startOffset;

    // Read element info
    SmHwStorageElementInfo elementInfo;
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)&elementInfo, sizeof(elementInfo));

    // Read data
    readData(elementInfo.offset + offset, pData, size);
}


inline uint8_t SmHwStorage::sendByte(uint8_t data)
{
    uint8_t ret;
    mSpi->transfer(&ret, &data, 1);
    return ret;
}

uint32_t SmHwStorage::readId(void)
{
    uint32_t result = 0;

    // Select the FLASH: Chip Select low
    mSpi->resetSs();

    // Send command
    sendByte(FLASH_CMD_READ_IDENTIFICATION);

    // Read 3 bytes
    result |= sendByte(FLASH_DUMMY_BYTE);
    result <<= 8;
    result |= sendByte(FLASH_DUMMY_BYTE);
    result <<= 8;
    result |= sendByte(FLASH_DUMMY_BYTE);

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();

    return result;
}

void SmHwStorage::readData(uint32_t offset, uint8_t *pData, uint32_t size)
{
    uint8_t res = 0;
    uint8_t data;

    // Select the FLASH: Chip Select low
    mSpi->resetSs();

    // Send "Read from Memory " instruction
    sendByte(FLASH_CMD_READ_DATA_BYTES);

    // Send offset high nibble address byte to read from
    sendByte((offset & 0xFF0000) >> 16);
    // Send offset medium nibble address byte to read from
    sendByte((offset & 0xFF00) >> 8);
    // Send offset low nibble address byte to read from
    sendByte(offset & 0xFF);

    data = FLASH_DUMMY_BYTE;
    while (size--) // while there is data to be read
    {
        // Read a byte from the FLASH
        mSpi->transfer(pData, &data, 1);

        // Point to the next location where the byte read will be saved
        pData++;
    }

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();
}

/// @brief Enable write.
void SmHwStorage::writeEnable(void)
{
    // Select the FLASH: Chip Select low
    mSpi->resetSs();

    // Send "Write Enable" instruction
    sendByte(FLASH_CMD_WRITE_ENABLE);

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();
}


/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void SmHwStorage::eraseSector(uint32_t SectorAddr)
{
    // Send write enable instruction
    writeEnable();

    // Sector Erase
    // Select the FLASH: Chip Select low
    mSpi->resetSs();
    // Send Sector Erase instruction
    sendByte(FLASH_CMD_SECTOR_ERASE);
    // Send SectorAddr high nibble address byte
    sendByte((SectorAddr & 0xFF0000) >> 16);
    // Send SectorAddr medium nibble address byte
    sendByte((SectorAddr & 0xFF00) >> 8);
    // Send SectorAddr low nibble address byte
    sendByte(SectorAddr & 0xFF);
    // Deselect the FLASH: Chip Select high
    mSpi->setSs();

    // Wait the end of Flash writing
    waitForWriteEnd();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void SmHwStorage::waitForWriteEnd(void)
{
    uint8_t flashstatus = 0;

    /*!< Select the FLASH: Chip Select low */
    mSpi->resetSs();

    /*!< Send "Read Status Register" instruction */
    sendByte(FLASH_CMD_READ_STATUS_REGISTER);

    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
        // Send a dummy byte to generate the clock needed by the FLASH
        // and put the value of the status register in FLASH_Status variable
        flashstatus = sendByte(FLASH_DUMMY_BYTE);

    }
    while ((flashstatus & FLASH_FLAG_WIP) == SET); /* Write in progress */

    // Deselect the FLASH: Chip Select high */
    mSpi->setSs();
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval None
  */
void SmHwStorage::eraseBulk(void)
{
    /*!< Send write enable instruction */
    writeEnable();

    /*!< Bulk Erase */
    /*!< Select the FLASH: Chip Select low */
    mSpi->resetSs();
    /*!< Send Bulk Erase instruction  */
    sendByte(FLASH_CMD_BULK_ERASE);
    /*!< Deselect the FLASH: Chip Select high */
    mSpi->setSs();

    /*!< Wait the end of Flash writing */
    waitForWriteEnd();
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  size: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void SmHwStorage::writePage(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t size)
{
    // Enable the write access to the FLASH
    writeEnable();

    mSpi->resetSs();

    // Send "Write to Memory " instruction
    sendByte(FLASH_CMD_PAGE_PROGRAM);

    // Send WriteAddr
    sendByte((WriteAddr & 0xFF0000) >> 16);
    sendByte((WriteAddr & 0xFF00) >> 8);
    sendByte(WriteAddr & 0xFF);

    while (size--)
    {
        sendByte(*pBuffer);
        pBuffer++;
    }

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();

    waitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  size: number of bytes to write to the FLASH.
  * @retval None
  */
void SmHwStorage::writeBuffer(uint8_t *pBuffer, unsigned int WriteAddr, unsigned int size)
{
    unsigned char NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % FLASH_SPI_PAGESIZE;
    count = FLASH_SPI_PAGESIZE - Addr;
    NumOfPage =  size / FLASH_SPI_PAGESIZE;
    NumOfSingle = size % FLASH_SPI_PAGESIZE;

    if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
    {
    if (NumOfPage == 0) /*!< size < sFLASH_PAGESIZE */
    {
        writePage(pBuffer, WriteAddr, size);
    }
    else /*!< size > sFLASH_PAGESIZE */
    {
        while (NumOfPage--)
        {
            writePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
            WriteAddr +=  FLASH_SPI_PAGESIZE;
            pBuffer += FLASH_SPI_PAGESIZE;
        }

        writePage(pBuffer, WriteAddr, NumOfSingle);
    }
    }
    else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
    {
        if (NumOfPage == 0) /*!< size < sFLASH_PAGESIZE */
        {
            if (NumOfSingle > count) /*!< (size + WriteAddr) > sFLASH_PAGESIZE */
            {
                temp = NumOfSingle - count;

                writePage(pBuffer, WriteAddr, count);
                WriteAddr +=  count;
                pBuffer += count;

                writePage(pBuffer, WriteAddr, temp);
            }
            else
            {
                writePage(pBuffer, WriteAddr, size);
            }
        }
        else /*!< size > sFLASH_PAGESIZE */
        {
            size -= count;
            NumOfPage =  size / FLASH_SPI_PAGESIZE;
            NumOfSingle = size % FLASH_SPI_PAGESIZE;

            writePage(pBuffer, WriteAddr, count);
            WriteAddr +=  count;
            pBuffer += count;

            while (NumOfPage--)
            {
                writePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
                WriteAddr +=  FLASH_SPI_PAGESIZE;
                pBuffer += FLASH_SPI_PAGESIZE;
            }

            if (NumOfSingle != 0)
            {
                writePage(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

void SmHwStorage::onSleep(void)
{
    mSpi->resetSs();
    sendByte(FLASH_CMD_POWERDOWN);
    mSpi->setSs();
}

void SmHwStorage::onWake(void)
{
    mSpi->resetSs();
    sendByte(FLASH_CMD_WAKE);
    /// @todo It would be fine to make a calibrated 3us delay.
    for (uint32_t i = 0; i < 1000; i++)
        mSpi->setSs();
}
#else
void SmHwStorage::init(void)
{
    inFile.open("../../Resources/Flash/update.bin", std::ios::in | std::ios::binary);

    // Read total number of elements in the storage
    readData(0, (uint8_t *)&mCount, sizeof(mCount));

    // Verify CRC
    uint32_t crc32 = SmCrc::calc32(0xFFFFFFFF, (uint8_t *)&mCount, sizeof(mCount));
    uint32_t addr = sizeof(mCount);
    SmHwStorageElementInfo elementInfo;
    // Next element offset. We can check if it is correct
    uint32_t nextOffset = sizeof(mCount) + sizeof(elementInfo)*mCount + sizeof(crc32);
    for (uint32_t i = 0; i < mCount; ++i)
    {
        // Read element info
        readData(addr, (uint8_t *)&elementInfo, sizeof(elementInfo));
        crc32 = SmCrc::calc32(crc32, (uint8_t *)&elementInfo, sizeof(elementInfo));
        addr += sizeof(elementInfo);

        if(elementInfo.offset != nextOffset)
        {
            mCount = 0;
            return;
        }

        // Calculate next element offset
        nextOffset += elementInfo.size;
    }
    uint32_t crcStored;
    readData(addr, (uint8_t *)&crcStored, sizeof(crcStored));
    if (crcStored != crc32)
        mCount = 0;
}

uint32_t SmHwStorage::getElementSize(uint8_t element)
{
    SmHwStorageElementInfo elementInfo;

    if (getElementInfo(element, &elementInfo) == false)
        return 0;

    return elementInfo.size;
}

bool SmHwStorage::getElementInfo(uint8_t element, SmHwStorageElementInfo * info)
{
    if (element >= mCount)
        return false;

    // Read element info
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)info, sizeof(SmHwStorageElementInfo));

    return true;
}

void SmHwStorage::readElement(uint8_t element, uint32_t offset, uint8_t * pData, uint32_t size)
{
    if (element >= mCount)
        return;

    // Read element info
    SmHwStorageElementInfo elementInfo;
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)&elementInfo, sizeof(elementInfo));

    // Read data
    readData(elementInfo.offset + offset, pData, size);
}

/// @todo Rewrite
void SmHwStorage::readData(uint32_t offset, uint8_t *pData, uint32_t size)
{
    // Reset text file to the beginning
    inFile.seekg(offset, std::ios::beg);

    // Read data
    inFile.read((char *)pData,size);
}
#endif