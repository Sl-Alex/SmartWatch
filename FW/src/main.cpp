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
#include "sm_hal_rtc.h"

#include "sm_hw_keyboard.h"
#include "sm_hw_motor.h"
#include "sm_hw_battery.h"
#include "sm_hw_bt.h"
#include "sm_hw_storage.h"
#include "sm_hw_powermgr.h"
#include "sm_hw_bmc150.h"
#include "sm_hw_bmp180.h"

#include "sm_desktop.h"
#include "sm_canvas.h"
#include "sm_image.h"
#include "sm_font.h"
#include "sm_animator.h"

#define I2C_ACC 0x10
#define I2C_MAGN 0x12

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

    SmHwBmc150::getInstance()->init();
    SmHwBmp180::getInstance()->init(); /// I2C init is done in BMC150 initialization

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

    SmHalI2c::getInstance()->reset(false);

    uint8_t data = 1;
    uint8_t reg[2] = {0x4b, 0x01};

    SmHalI2c::getInstance()->transfer(I2C_MAGN, &reg[0], 2, 0, 0);

    reg[0] = 0x40;
    // 0b00110010;
    SmHalI2c::getInstance()->transfer(I2C_MAGN, &reg[0], 1, &data, 1);

    SmHwBmc150::getInstance()->checkPresent();
    SmHwBmp180::getInstance()->checkPresent();

    SmDesktop::getInstance()->init();
    while (1)
    {
        SmHalSysTimer::processEvents();
        display->update();
        SmHwBt::getInstance()->update();
        if (SmHwBt::getInstance()->isNotification())
        {
            display->powerOn();
            SmNotification * pNotification = SmDesktop::getInstance()->getNotification();

            if (pNotification)
                pNotification->addCount();
            else
                SmDesktop::getInstance()->showNotification();

            // Show "SOS" pattern (...---...)
            motor->startNotification(0x0FC0,9);
            SmHwBt::getInstance()->clearNotification();
        }
        SmHwPowerMgr::getInstance()->updateState();
    }
}
