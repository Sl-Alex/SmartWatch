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
#include "sm_display.h"
#include "sm_hal_gpio.h"
#include "sm_hal_spi_sw.h"
#include "sm_hal_spi_hw.h"
#include "sm_hal_rcc.h"
#include "sm_hal_rtc.h"

#include "sm_hw_keyboard.h"
#include "sm_hw_motor.h"
#include "sm_hw_battery.h"
#include "sm_hw_bt.h"
#include "sm_hw_storage.h"
#include "sm_hw_powermgr.h"

#include "sm_canvas.h"

// User app will be located at 16K
#define USER_APP_START_ADDR 0x4000

int main(void)
{
    SystemCoreClockUpdate();
    SmHalRcc::updateClocks();

    SmHalRcc::clockEnable(RCC_PERIPH_SPI2);
    SmHalRcc::clockEnable(RCC_PERIPH_GPIOA);
    SmHalRcc::clockEnable(RCC_PERIPH_GPIOB);
    SmHalRcc::clockEnable(RCC_PERIPH_GPIOC);
    SmHalRcc::clockEnable(RCC_PERIPH_AFIO);
    SmHalRcc::clockEnable(RCC_PERIPH_ADC1);
    SmHalRcc::clockEnable(RCC_PERIPH_I2C2);

    // Disable JTAG, SWD remains enabled
    // This is for PA15, which is JTDI by default
    AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    // Initialize system timer (10 clients, 1ms resolution)
    SmHalSysTimer::initSubscribersPool(10);
    // Initialize with 1ms resolution
    SmHalSysTimer::init(1);

    SmHalRtc::getInstance()->init();

    // Initialize power manager (10 clients)
    SmHwPowerMgr::getInstance()->initSubscribersPool(10);
    // Initialize wake lines
    SmHwPowerMgr::getInstance()->init();

    // If update is not requested by the SW ans no buttons are pressed
    if ((SmHwKeyboard::getInstance()->getState() == 0) && (!SmHalRtc::getInstance()->isUpdateRequested()))
    {
        /// TODO: Check user SW, run if any
        SCB->VTOR = USER_APP_START_ADDR;
        __set_MSP(*((volatile uint32_t*) USER_APP_START_ADDR)); //stack pointer (to RAM) for USER app in this address
        // Jump to app
        ((void (*)(void))(USER_APP_START_ADDR + 4))();
    }

    // Prepare for FW update
    SmHwBt::getInstance()->init();

    SmHwBattery::getInstance()->init();

    SmHwMotor * motor = new SmHwMotor();

    SmHwStorage::getInstance()->init();

    // Initialize display
    SmDisplay * display = SmDisplay::getInstance();
    display->init(128,64);
    display->getCanvas()->clear();
    display->powerOn();
    display->update();

    display->getCanvas()->drawRect(16, 40, 111, 48, 1);

    while (1)
    {
        SmHalSysTimer::processEvents();
        display->update();
        SmHwBt::getInstance()->update();
        SmHwPowerMgr::getInstance()->updateState();
    }
}
