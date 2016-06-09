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

// Display SPI interface
typedef SmHwSpiSw<SM_HW_SPI_MODE0,SM_HW_SPI_CFG_OUT,SM_HW_SPI_WIDTH_8> DisplaySpi;
typedef SmHwSpiHw<SPI2_BASE, SM_HW_SPI_CFG_FULL_DUPLEX> MemorySpi;

int main(void)
{
    // Initialize display memory
    SmDisplay * display = new SmDisplay();
    display->init(128,64);

    // Initialize display interface
    DisplaySpi * spi = new DisplaySpi();
    spi->setSsPins(new SmHwGpio<GPIOA_BASE,1>(), 1);
    spi->init(new SmHwGpio<GPIOA_BASE,3>(),     /// SCK
              0,                                /// MISO - not used in SM_HW_SPI_CFG_OUT configuration
              new SmHwGpio<GPIOA_BASE,2>());    /// MOSI

    // Initialize SPI flash
    MemorySpi * spiMem = new MemorySpi();
    spiMem->setSsPins(new SmHwGpio<GPIOB_BASE,12>(), 1);
    spiMem->init(SM_HW_SPI_MODE3, SM_HW_SPI_WIDTH_8);

    // Apply display interface
    display->setInterface(spi, new SmHwGpio<GPIOA_BASE,0>());

    // Do something
    display->setPix(20,20,1);
    display->update();
    while (1)
    {
        display->update();
    }
}
