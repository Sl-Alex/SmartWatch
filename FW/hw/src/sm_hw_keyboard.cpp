#include "sm_hw_keyboard.h"

#include "sm_hal_gpio.h"

SmHwKeyboard::SmHwKeyboard(void)
{
    // Construct pins
    mGpioPins[0] = new SmHalGpio<GPIOA_BASE, 12>();
    mGpioPins[1] = new SmHalGpio<GPIOB_BASE, 3>();
    mGpioPins[2] = new SmHalGpio<GPIOA_BASE, 1>();
    mGpioPins[3] = new SmHalGpio<GPIOA_BASE, 6>();

    // Initialize as inputs
    for (uint8_t i = 0; i < 4; ++i)
    {
        mGpioPins[i]->setModeSpeed(SM_HAL_GPIO_MODE_IN_PU, SM_HAL_GPIO_SPEED_2M);
        mLastState[i] = false;
    }

    SmHalSysTimer::subscribe(this, DEBOUNCING_TIME, true);
    SmHwPowerMgr::getInstance()->subscribe(this);
}

void SmHwKeyboard::onSleep(void)
{
    for (uint8_t i = 0; i < 4; ++i)
    {
        mLastState[i] = false;
    }
}

void SmHwKeyboard::onWake(void)
{
    // Real timestamp does not matter in this case;
    onTimer(0);
}

void SmHwKeyboard::onTimer(uint32_t timeStamp)
{
    for (uint8_t i = 0; i < 4; ++i)
    {
        mLastState[i] = !mGpioPins[i]->readPin();
    }
}
