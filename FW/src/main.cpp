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

// Display SPI interface
typedef SmHwSpiSw<SM_HW_SPI_MODE0,SM_HW_SPI_CFG_OUT,SM_HW_SPI_WIDTH_8> DisplaySpi;

int main(void)
{
    // Initialize display memory
    SmDisplay * display = new SmDisplay();
    display->init(128,64);

    // Initialize display interface
    DisplaySpi * spi = new DisplaySpi();
    spi->setSsPins(new SmHwGpioPin(GPIOA,1), 1);
    spi->init(new SmHwGpioPin(GPIOA,3),     /// SCK
              0,                            /// MISO - not used in SM_HW_SPI_CFG_OUT configuration
              new SmHwGpioPin(GPIOA,2));    /// MOSI

    // Apply display interface
    display->setInterface(spi, new SmHwGpioPin(GPIOA,0));

    // Do something
    display->setPix(20,20,1);
    display->update();
    while (1)
    {
        display->update();
    }
}
