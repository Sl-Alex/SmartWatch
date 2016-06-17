#include "sm_hw_bt.h"
#include "sm_hal_gpio.h"

void SmHwBt::init(void)
{
    // Initialize power pin
    mPowerPin = new SmHalGpio<GPIOB_BASE, 4>();
    // Initialize default BT state
    disable();
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
