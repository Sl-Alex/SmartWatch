#ifndef SM_HAL_ABSTRACT_GPIO_H_INCLUDED
#define SM_HAL_ABSTRACT_GPIO_H_INCLUDED

#include "stm32f10x.h"

enum GpioSpeed
{
    SM_HAL_GPIO_SPEED_10M = 1,
    SM_HAL_GPIO_SPEED_2M,
    SM_HAL_GPIO_SPEED_50M
};
enum GpioMode
{
    SM_HAL_GPIO_MODE_AIN      = 0,
    SM_HAL_GPIO_MODE_IN_FLOAT = 0x04,
    SM_HAL_GPIO_MODE_IN_PD    = 0x28,
    SM_HAL_GPIO_MODE_IN_PU    = 0x48,
    SM_HAL_GPIO_MODE_OUT_OD   = 0x14,
    SM_HAL_GPIO_MODE_OUT_PP   = 0x10,
    SM_HAL_GPIO_MODE_AF_OD    = 0x1C,
    SM_HAL_GPIO_MODE_AF_PP    = 0x18
};

/// @brief GPIO abstraction
class SmHalAbstractGpio
{
public:

    SmHalAbstractGpio()
        :mMode(SM_HAL_GPIO_MODE_IN_FLOAT),
        mSpeed(SM_HAL_GPIO_SPEED_2M)
    {}

    /// @brief Set both GPIO mode and speed
    virtual void setModeSpeed(GpioMode mode, GpioSpeed speed) = 0;

    /// @brief Set GPIO mode
    virtual void setMode(GpioMode mode)
    {
        mMode = mode;
        setModeSpeed(mMode, mSpeed);
    }

    /// @brief set GPIO speed
    virtual void setSpeed(GpioSpeed speed)
    {
        mSpeed = speed;
        setModeSpeed(mMode, mSpeed);
    }

    /// @brief Set pin
    virtual void setPin(void) = 0;
    /// @brief Reset pin
    virtual void resetPin(void) = 0;
    /// @brief Read pin state
    virtual uint8_t readPin(void) = 0;

private:
    GpioMode mMode;
    GpioSpeed mSpeed;
};

#endif /* SM_HAL_ABSTRACT_GPIO_H_INCLUDED */
