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
    AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    // Init system timer and power manager
    SmHalSysTimer::initSubscribersPool(10);
    SmHalSysTimer::init(1); ///< 1ms resolution
    SmHwPowerMgr::getInstance()->initSubscribersPool(10);
    SmHwPowerMgr::getInstance()->init(); ///< Initialize wake lines

    SmHwBt::getInstance()->init(new SmHalGpio<GPIOB_BASE, 4>());


    SmHwBattery * battery = SmHwBattery::getInstance();
    battery->init();

    // Initialize display memory
    SmDisplay * display = new SmDisplay();

    SmHwKeyboard *keyboard = new SmHwKeyboard();

    SmHwMotor * motor = new SmHwMotor();
    SmHalSysTimer::subscribe(motor,1000,true);

    SmHwStorage::getInstance()->init();
    SmHwStorage::getInstance()->readId();

    // Apply display interface
    display->init(128,64);

    // Do something
    display->setPix(20,20,1);
    display->update();

    while (1)
    {
        SmHalSysTimer::processEvents();
        display->update();

        if (keyboard->getState(2) && keyboard->getState(3))
        {
            SmHwPowerMgr::getInstance()->sleep();
        }
    }
}
