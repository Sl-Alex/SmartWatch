#include "sm_hw_storage.h"
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


void SmHwStorage::init(void)
{
    mSpi = new SmHalSpiHw<SPI_BASE, SM_HAL_SPI_CFG_FULL_DUPLEX>();

    SmHalGpio<MOSI_PORT_BASE, MOSI_PIN> spiMosi;
    SmHalGpio<MISO_PORT_BASE, MISO_PIN> spiMiso;
    SmHalGpio<SCK_PORT_BASE,  SCK_PIN>  spiSck;

    spiMosi.setModeSpeed(SM_HAL_GPIO_MODE_AF_PP,SM_HAL_GPIO_SPEED_50M);
    spiMiso.setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT,SM_HAL_GPIO_SPEED_50M);
    spiSck.setModeSpeed(SM_HAL_GPIO_MODE_AF_PP,SM_HAL_GPIO_SPEED_50M);

    mSpi->setSsPins(new SmHalGpio<SS_PORT_BASE, SS_PIN>(), 1);
    ((SmHalSpiHw<SPI_BASE, SM_HAL_SPI_CFG_FULL_DUPLEX> *)mSpi)->init(SM_HAL_SPI_MODE3, SM_HAL_SPI_WIDTH_8);
}

uint32_t SmHwStorage::readId(void)
{
    uint32_t result = 0;
    uint32_t temp = 0;
    uint8_t data;

    // Select the FLASH: Chip Select low
    mSpi->resetSs();

    // Send command
    data = FLASH_CMD_READ_IDENTIFICATION;
    mSpi->transfer(&temp, &data, 1);

    // Read 3 bytes
    data = FLASH_DUMMY_BYTE;
    mSpi->transfer(&temp, &data, 1);
    result |= temp;
    result <<= 8;
    mSpi->transfer(&temp, &data, 1);
    result |= temp;
    result <<= 8;
    mSpi->transfer(&temp, &data, 1);
    result |= temp;

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();

    return result;
}
