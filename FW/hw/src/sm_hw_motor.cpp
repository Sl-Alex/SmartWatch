#include "sm_hw_motor.h"

#include "sm_hal_gpio.h"

#define MOTOR_PORT  GPIOA_BASE
#define MOTOR_PIN   8

SmHwMotor::SmHwMotor()
{
    mGpio = new SmHalGpio<MOTOR_PORT, MOTOR_PIN>();
    mGpio->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);
    mGpio->resetPin();
    SmHwPowerMgr::getInstance()->subscribe(this);
}

void SmHwMotor::onSleep(void)
{
    mGpio->resetPin();
}

void SmHwMotor::onWake(void)
{
    mGpio->setPin();
}

void SmHwMotor::onTimer(uint32_t timeStamp)
{
    /// @todo Implement correct behaviour
    static uint8_t tmp = 0;
    tmp = 1 - tmp;

    if (tmp)
        mGpio->setPin();
    else
        mGpio->resetPin();

}
