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

#include "sm_hw_keyboard.h"
#include "sm_hw_motor.h"
#include "sm_hw_battery.h"
#include "sm_hw_bt.h"
#include "sm_hw_storage.h"
#include "sm_hw_powermgr.h"

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
    // This is for PA15, which is JTDI by default
    AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    // Initialize system timer (10 clients, 1ms resolution)
    SmHalSysTimer::initSubscribersPool(10);
    // Initialize with 1ms resolution
    SmHalSysTimer::init(1);
    // Initialize power manager (10 clients)
    SmHwPowerMgr::getInstance()->initSubscribersPool(10);
    // Initialize wake lines
    SmHwPowerMgr::getInstance()->init();

    SmHwBt::getInstance()->init(new SmHalGpio<GPIOB_BASE, 4>());


    SmHwBattery::getInstance()->init();

    // Initialize display memory
    SmDisplay * display = new SmDisplay();

    SmHwKeyboard *keyboard = new SmHwKeyboard();

    SmHwMotor * motor = new SmHwMotor();
    SmHalSysTimer::subscribe(motor,1000,true);

    SmHwStorage::getInstance()->init();

    // Apply display interface
    display->init(128,64);

    // Do something
    display->setPix(20,20,1);
    display->update();

    while (1)
    {
        SmHalSysTimer::processEvents();
        display->update();

        SmHwBattery::getInstance()->getValue();
        if (keyboard->getState(2) && keyboard->getState(3))
        {
            SmHwPowerMgr::getInstance()->sleep();
        }
    }
}
