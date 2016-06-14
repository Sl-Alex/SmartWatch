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
//#include "stm32f10x_conf.h"
#include "sm_display.h"
#include "sm_hal_gpio.h"
#include "sm_hal_spi_sw.h"
#include "sm_hal_spi_hw.h"
#include "sm_hal_rcc.h"

#include "sm_hw_button.h"
#include "sm_hw_motor.h"
#include "sm_hw_battery.h"
#include "sm_hw_bt.h"
#include "sm_hw_storage.h"
#include "sm_hw_powermgr.h"

// Display SPI interface
typedef SmHalSpiSw<SM_HAL_SPI_MODE0, SM_HAL_SPI_CFG_OUT, SM_HAL_SPI_WIDTH_8> DisplaySpi;

int main(void)
{
    SystemCoreClockUpdate();
    SmHalRcc::RccClockEnable(RCC_PERIPH_SPI2);
    SmHalRcc::RccClockEnable(RCC_PERIPH_GPIOA);
    SmHalRcc::RccClockEnable(RCC_PERIPH_GPIOB);
    SmHalRcc::RccClockEnable(RCC_PERIPH_GPIOC);
    SmHalRcc::RccClockEnable(RCC_PERIPH_AFIO);
    SmHalRcc::RccClockEnable(RCC_PERIPH_ADC1);

    // Disable JTAG, SWD remains enabled
    AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    SmHwBt::getInstance()->init(new SmHalGpio<GPIOB_BASE, 4>());

    SmHalSysTimer::initSubscribersPool(10);
    SmHalSysTimer::init(1);

    SmHwBattery * battery = SmHwBattery::getInstance();
    battery->init();

    // Initialize display memory
    SmDisplay * display = new SmDisplay();

    SmHwButton *button1 = new SmHwButton();
    SmHwButton *button2 = new SmHwButton();
    SmHwButton *button3 = new SmHwButton();
    SmHwButton *button4 = new SmHwButton();
    button1->init(new SmHalGpio<GPIOA_BASE, 12>());
    button2->init(new SmHalGpio<GPIOB_BASE, 3>());
    button3->init(new SmHalGpio<GPIOA_BASE, 1>());
    button4->init(new SmHalGpio<GPIOA_BASE, 6>());

    {
        SmHalAbstractGpio * BatGpio = new SmHalGpio<GPIOA_BASE, 0>();
        BatGpio->setModeSpeed(SM_HAL_GPIO_MODE_AIN, SM_HAL_GPIO_SPEED_2M);
    }

    SmHwMotor * motor = new SmHwMotor();
    motor->init(new SmHalGpio<GPIOA_BASE, 8>());
    SmHalSysTimer::subscribe(motor,1000,true);

    // Initialize display interface
    DisplaySpi * spi = new DisplaySpi();
    SmHalRcc::RccClockEnable(RCC_PERIPH_GPIOA);
    spi->setSsPins(new SmHalGpio<GPIOB_BASE,5>(), 1);
    // Initialize display SPI pins
    {
        //SmHalGpio<GPIOB_BASE,8> * spiSck  = new SmHalGpio<GPIOB_BASE,8>();
        //SmHalGpio<GPIOB_BASE,9> * spiMosi = new SmHalGpio<GPIOB_BASE,9>();
        /// @todo Do something with this pin
        SmHalGpio<GPIOB_BASE,6> * dispRes = new SmHalGpio<GPIOB_BASE,6>();
        dispRes->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP,SM_HAL_GPIO_SPEED_50M);
        dispRes->setPin();
        spi->init(new SmHalGpio<GPIOB_BASE,8>(),    /// SCK
                  0,                                /// MISO - not used in SM_HW_SPI_CFG_OUT configuration
                  new SmHalGpio<GPIOB_BASE,9>());   /// MOSI
    }

    SmHwStorage::getInstance()->init();
    SmHwStorage::getInstance()->readId();

    // Apply display interface
    display->init(128,64,spi,new SmHalGpio<GPIOB_BASE,7>(), new SmHalGpio<GPIOC_BASE,13>());

    // Do something
    display->setPix(20,20,1);
    display->update();
    SmHalAbstractGpio * batEn = new SmHalGpio<GPIOA_BASE, 2>();
    batEn->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);
    batEn->resetPin();

    SmHwPowerMgr::getInstance()->init();
    while (1)
    {
        batEn->setPin();
        battery->getValue();
        batEn->resetPin();
        battery->getValue();
        SmHalSysTimer::processEvents();
        if (button1->getState())
        {
            display->fill(0x01);
        }
        else if (button2->getState())
        {
            display->fill(0x05);
        }
        else if (button3->getState())
        {
            display->fill(0x0F);
        }
        else if (button4->getState())
        {
            display->fill(0xAA);
        }
        else
        {
            display->update();
        }

        if (button2->getState() && button3->getState())
        {
            SmHwPowerMgr::getInstance()->sleep();
        }
    }
}
