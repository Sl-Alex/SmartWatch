#ifndef SM_HW_GPIO_H_INCLUDED
#define SM_HW_GPIO_H_INCLUDED

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

class SmHwGpioPin
{
public:

    SmHwGpioPin(GPIO_TypeDef * gpio, uint8_t pin)
        :mMode(SM_HW_GPIO_MODE_IN_FLOAT),
        mSpeed(SM_HW_GPIO_SPEED_2M),
        mGpio(gpio),
        mPin(pin)
    {}

    void setModeSpeed(GpioMode mode, GpioSpeed speed);

    inline void setMode(GpioMode mode);
    inline void setSpeed(GpioSpeed speed);
    inline void setPin(void)
    {
        mGpio->BSRR = (1UL << mPin);
    }
    inline void resetPin(void)
    {
        mGpio->BRR = (1UL << mPin);
    }
    uint8_t readPin(void);

private:
    GpioMode mMode;
    GpioSpeed mSpeed;
    GPIO_TypeDef * mGpio;
    uint8_t  mPin;
};

#endif /* SM_HW_GPIO_H_INCLUDED */
