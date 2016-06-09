#ifndef SM_HW_ABSTRACT_GPIO_H_INCLUDED
#define SM_HW_ABSTRACT_GPIO_H_INCLUDED

#include "stm32f10x.h"

enum GpioSpeed
{
    SM_HW_GPIO_SPEED_10M = 1,
    SM_HW_GPIO_SPEED_2M,
    SM_HW_GPIO_SPEED_50M
};
enum GpioMode
{
    SM_HW_GPIO_MODE_AIN      = 0,
    SM_HW_GPIO_MODE_IN_FLOAT = 0x04,
    SM_HW_GPIO_MODE_IN_PD    = 0x28,
    SM_HW_GPIO_MODE_IN_PU    = 0x48,
    SM_HW_GPIO_MODE_OUT_OD   = 0x14,
    SM_HW_GPIO_MODE_OUT_PP   = 0x10,
    SM_HW_GPIO_MODE_AF_OD    = 0x1C,
    SM_HW_GPIO_MODE_AF_PP    = 0x18
};

class SmHwAbstractGpio
{
public:

    SmHwAbstractGpio()
        :mMode(SM_HW_GPIO_MODE_IN_FLOAT),
        mSpeed(SM_HW_GPIO_SPEED_2M)
    {}

    virtual void setModeSpeed(GpioMode mode, GpioSpeed speed) = 0;

    virtual void setMode(GpioMode mode)
	{
		mMode = mode;
		setModeSpeed(mMode, mSpeed);
	}
    virtual void setSpeed(GpioSpeed speed)
	{
		mSpeed = speed;
		setModeSpeed(mMode, mSpeed);
	}
    virtual void setPin(void) = 0;
    virtual void resetPin(void) = 0;
    virtual uint8_t readPin(void) = 0;

private:
    GpioMode mMode;
    GpioSpeed mSpeed;
};

#endif /* SM_HW_ABSTRACT_GPIO_H_INCLUDED */
