#include "sm_hw_motor.h"

#include "sm_hal_gpio.h"

#define MOTOR_PORT  GPIOA_BASE
#define MOTOR_PIN   8

/// "Tick" of the motor, ms
#define MOTOR_TICK  80

#define SHORT_SLOT  MOTOR_TICK
#define LONG_SLOT   MOTOR_TICK*3

SmHwMotor::SmHwMotor()
{
    mGpio = new SmHalGpio<MOTOR_PORT, MOTOR_PIN>();
    mGpio->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);
    mPattern = 0;
    mLength = 0;
    SmHwPowerMgr::getInstance()->subscribe(this);
    disable();
}

void SmHwMotor::onSleep(void)
{
    disable();
}

void SmHwMotor::onWake(uint32_t wakeSource)
{
}

void SmHwMotor::onTimer(uint32_t timeStamp)
{
    if (mPattern & 1)
        SmHalSysTimer::subscribe(this, LONG_SLOT, false);
    else
        SmHalSysTimer::subscribe(this, SHORT_SLOT, false);

    if (mLength & 1)
        enable();
    else
        disable();


    mPattern >>= 1;

    if (mLength)
        mLength--;
    else
        stopNotification();
}

void SmHwMotor::startNotification(uint32_t pattern, uint8_t pulses)
{
    if ((pulses == 0) || (pulses > 16))
        return;

    mPattern = pattern;
    mLength = pulses * 2 - 1;

    // Call onTimer with any value
    onTimer(0);
}

void SmHwMotor::stopNotification(void)
{
    SmHalSysTimer::unsubscribe(this);
	disable();
}
