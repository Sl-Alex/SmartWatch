#include "sm_hw_button.h"

void SmHwButton::init(SmHalAbstractGpio * pin)
{
    mGpio = pin;
    mGpio->setModeSpeed(SM_HAL_GPIO_MODE_IN_PU, SM_HAL_GPIO_SPEED_2M);
    mLastState = false;
    SmHalSysTimer::subscribe(this, DEBOUNCING_TIME, true);
}

/// @todo Implement complete mechanism
void SmHwButton::onTimer(uint32_t timeStamp)
{
    bool newState = !mGpio->readPin();
    if (newState != mLastState)
    {
        mLastState = newState;
    }
}
