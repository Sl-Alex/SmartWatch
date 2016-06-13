#include "sm_hw_bt.h"

void SmHwBt::init(SmHalAbstractGpio * powerPin)
{
    // Initialize power pin
    mPowerPin = powerPin;
    // Initialize default BT state
    enable();
    // Set as output
    mPowerPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);
}

void SmHwBt::enable(void)
{
    mPowerPin->resetPin();
}

void SmHwBt::disable(void)
{
    mPowerPin->setPin();
}
