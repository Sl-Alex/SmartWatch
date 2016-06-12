#include "sm_hw_motor.h"

void SmHwMotor::init(SmHalAbstractGpio * pin)
{
    mGpio = pin;
    mGpio->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);
    mGpio->resetPin();
}

void SmHwMotor::onTimer(uint32_t timeStamp)
{
    static uint8_t tmp = 0;
    tmp = 1 - tmp;

    if (tmp)
        mGpio->setPin();
    else
        mGpio->resetPin();

}
