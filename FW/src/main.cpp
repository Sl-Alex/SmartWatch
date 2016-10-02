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

    // Initialize display memory
    SmDisplay * display = new SmDisplay();

    SmHwKeyboard *keyboard = new SmHwKeyboard();
    keyboard->initSubscribersPool(10);

    SmHwMotor * motor = new SmHwMotor();
    keyboard->subscribe(motor);
    SmHalSysTimer::subscribe(motor,2000,true);

    SmHwStorage::getInstance()->init();

    // Apply display interface
    display->init(128,64);
    display->getCanvas()->clear();

    SmHalI2c::getInstance()->reset(false);

    // Do something
//    display->setPix(20,20,1);
    display->update();

    uint8_t data = 1;
    uint8_t reg[2] = {0x4b, 0x01};

    SmHalI2c::getInstance()->transfer(I2C_MAGN, &reg[0], 2, 0, 0);

    reg[0] = 0x40;
    // 0b00110010;
    SmHalI2c::getInstance()->transfer(I2C_MAGN, &reg[0], 1, &data, 1);

//    SmHalI2c::getInstance()->reset();
    SmHwBmc150::getInstance()->checkPresent();
    SmHwBmp180::getInstance()->checkPresent();
    SmHalRtc::init();


    //reg[0] = 0x00;
    // 0b11111010;
    //SmHalI2c::getInstance()->transfer(I2C_ACC, &reg[0], 1, &data, 1);

    SmFont * smallFont = new SmFont();
    smallFont->init(IDX_FW_FONT_SMALL);

    SmHalRtc::setCounter(17*3600 + 59*60 + 50);

    SmDesktop::getInstance()->init(display->getCanvas());
    while (1)
    {
        SmHalSysTimer::processEvents();
/*        uint16_t txt_time[10] = {0x10,0x58,0,0,0,0,0,0};
        for (uint8_t i = 0; i < 8; i++)
        {
            uint32_t dig = rtc & 0x0F;
            rtc >>= 4;
            if (dig < 0x0A)
                dig = 0x10 + dig;
            else
                dig = 0x21 + dig - 0x0A;

            txt_time[9 - i] = dig;
        }
        display->getCanvas()->fillRect(0,56,127,63,0);
        smallFont->drawText(display->getCanvas(), 0, 56, txt_time, 10);
*/
        display->update();

        uint8_t st1 = keyboard->getState(1);
        uint8_t st2 = keyboard->getState(2) << 1;
        uint8_t st3 = keyboard->getState(3) << 2;
        uint8_t st4 = keyboard->getState(4) << 3;

        uint8_t st = st1|st2|st3|st4;
        if (st)
        {
            SmHwBt::getInstance()->send(0x30+st);
        }

        SmHwBattery::getInstance()->getCharge();
        if (keyboard->getState(1) && keyboard->getState(2))
        {
            SmHwPowerMgr::getInstance()->sleep();
        }
    }
}
