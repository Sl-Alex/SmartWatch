#include "sm_hw_gpio.h"

void SmHwGpioPin::setModeSpeed(GpioMode mode, GpioSpeed speed)
{
    mMode = mode;
    mSpeed = speed;

    uint32_t currentmode = mMode & 0x0F;
    // If it's an output pin
    if ((mMode & 0x10) != 0)
    {
        /* Output mode - can set speed*/
        currentmode |= mSpeed;
    }
    if (mPin < 8)
    {
        // Clear config
        mGpio->CRL &= ~(0xFUL << (mPin << 2));
        // Set config
        mGpio->CRL |= (currentmode << (mPin << 2));
    }
    else
    {
        // Clear config
        mGpio->CRH &= ~(0xFUL << ((mPin - 8) << 2));
        // Set config
        mGpio->CRH |= (currentmode << ((mPin - 8) << 2));
    }

    // Set pull-up/down
    if (mMode == SM_HW_GPIO_MODE_IN_PD)
    {
        mGpio->BRR = (1UL << mPin);
    }
    else
    {
        if (mMode == SM_HW_GPIO_MODE_IN_PU)
        {
            mGpio->BSRR = (1UL << mPin);
        }
    }
}

void SmHwGpioPin::setMode(GpioMode mode)
{
    setModeSpeed(mode,mSpeed);
}

void SmHwGpioPin::setSpeed(GpioSpeed speed)
{
    setModeSpeed(mMode,speed);
}

uint8_t SmHwGpioPin::readPin(void)
{
    if (mGpio->IDR & (1UL << mPin))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
