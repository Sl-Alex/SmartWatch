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
#include "sm_hw_bmc150.h"

#include "sm_canvas.h"
#include "sm_image.h"
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

    SmHwMotor * motor = new SmHwMotor();
    SmHalSysTimer::subscribe(motor,1000,true);

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

    //reg[0] = 0x00;
    // 0b11111010;
    //SmHalI2c::getInstance()->transfer(I2C_ACC, &reg[0], 1, &data, 1);

    SmImage * pCanvas = new SmImage();
    pCanvas->init(5);

//    SmCanvas * pCanvas = new SmCanvas();
//    pCanvas->init(32,32);
    pCanvas->clear();
    pCanvas->drawRect(0,0,31,31,1);
    display->getCanvas()->drawCanvas(0,0,pCanvas);

    SmAnimator * pAnimator= new SmAnimator();
    pAnimator->setDestSource(display->getCanvas(),pCanvas);
    pAnimator->setType(SmAnimator::AnimType::ANIM_TYPE_VIS_SLIDE);
    pAnimator->setDirection(SmAnimator::AnimDir::ANIM_DIR_LEFT);
    pAnimator->setSpeed(2);
    pAnimator->start(48,16,0,0,32,32);

    int xOff = 0;
    int yOff = 0;
    while (1)
    {
//        xOff++;
//        yOff++;
//        xOff &= 0x1F;
//        yOff &= 0x1F;
        if (!pAnimator->tick())
        {
            static bool dir = true;
            static bool step = true;
            dir = !dir;
            if (dir)
            {
                pAnimator->setDirection(SmAnimator::AnimDir::ANIM_DIR_LEFT);
                pAnimator->setSpeed(3);
            }
            else
            {
                pAnimator->setDirection(SmAnimator::AnimDir::ANIM_DIR_RIGHT);
                pAnimator->setSpeed(1);
                step = ! step;
            }
            if (step)
            {
                pCanvas->init(0);
                //pCanvas->fill(0x00);
                //pCanvas->drawRect(0,0,31,31,1);
            }
            else
            {
                pCanvas->fill(0x00);
                pCanvas->drawRect(0,0,31,31,1);
            }
            pAnimator->start(48-dir,16,0,0,32,32);
        }

//        display->getCanvas()->drawCanvas(32,32,xOff,yOff,32,32,pCanvas);

        SmHalSysTimer::processEvents();
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

        SmHwBattery::getInstance()->getValue();
        if (keyboard->getState(2) && keyboard->getState(3))
        {
            SmHwPowerMgr::getInstance()->sleep();
        }
    }
}
