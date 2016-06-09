/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f10x_conf.h"
#include "sm_display.h"
#include "sm_hw_gpio.h"
#include "sm_hw_spi_sw.h"
#include "sm_hw_spi_hw.h"
#include "sm_hw_rcc.h"

// Display SPI interface
typedef SmHwSpiSw<SM_HW_SPI_MODE0,SM_HW_SPI_CFG_OUT,SM_HW_SPI_WIDTH_8> DisplaySpi;
typedef SmHwSpiHw<SPI2_BASE, SM_HW_SPI_CFG_FULL_DUPLEX> MemorySpi;

int main(void)
{
    // Initialize display memory
    SmDisplay * display = new SmDisplay();

    // Initialize display interface
    DisplaySpi * spi = new DisplaySpi();
    SmHwRcc::RccClockEnable(RCC_PERIPH_GPIOA);
    spi->setSsPins(new SmHwGpio<GPIOA_BASE,1>(), 1);
    spi->init(new SmHwGpio<GPIOA_BASE,3>(),     /// SCK
              0,                                /// MISO - not used in SM_HW_SPI_CFG_OUT configuration
              new SmHwGpio<GPIOA_BASE,2>());    /// MOSI

    // Enable flash SPI/GPIO clocking
    SmHwRcc::RccClockEnable(RCC_PERIPH_SPI2);
    SmHwRcc::RccClockEnable(RCC_PERIPH_GPIOB);
    MemorySpi * spiMem = new MemorySpi();

    // Initialize flash SPI pins
    {
        SmHwGpio<GPIOB_BASE,15> spiMosi;
        SmHwGpio<GPIOB_BASE,14> spiMiso;
        SmHwGpio<GPIOB_BASE,13> spiSck;
        spiMosi.setModeSpeed(SM_HW_GPIO_MODE_AF_PP,SM_HW_GPIO_SPEED_50M);
        spiMiso.setModeSpeed(SM_HW_GPIO_MODE_IN_FLOAT,SM_HW_GPIO_SPEED_50M);
        spiSck.setModeSpeed(SM_HW_GPIO_MODE_AF_PP,SM_HW_GPIO_SPEED_50M);
    }
    // Initialize flash SPI HW
    spiMem->setSsPins(new SmHwGpio<GPIOB_BASE,12>(), 1);
    spiMem->init(SM_HW_SPI_MODE3, SM_HW_SPI_WIDTH_8);

    /// @todo Just a test
    /*!< Select the FLASH: Chip Select low */
    spiMem->resetSs();

    /*!< Send "RDID " instruction */
    uint8_t dataOut;
    uint32_t Temp[3] = {0, 0, 0};
    dataOut = 0x9F;
    spiMem->transfer(&Temp[0], &dataOut, 1);
    /*!< Read a byte from the FLASH */
    spiMem->transfer(&Temp[0], &Temp[0], 1);
    /*!< Read a byte from the FLASH */
    spiMem->transfer(&Temp[1], &Temp[0], 1);
    /*!< Read a byte from the FLASH */
    spiMem->transfer(&Temp[2], &Temp[0], 1);

    /*!< Deselect the FLASH: Chip Select high */
    spiMem->setSs();

    uint32_t Result = 0;
    Result = (Temp[0] << 16) | (Temp[1] << 8) | Temp[2];

    // Apply display interface
    /// @todo Check power pin
    display->init(128,64,spi,new SmHwGpio<GPIOA_BASE,0>(), new SmHwGpio<GPIOA_BASE,1>());

    // Do something
    display->setPix(20,20,1);
    display->update();
    while (1)
    {
        display->update();
    }
}
