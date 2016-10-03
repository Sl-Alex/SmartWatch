#ifndef SM_HAL_GPIO_H
#define SM_HAL_GPIO_H

#include "stm32f10x.h"
#include "sm_hal_abstract_gpio.h"

/// @brief GPIO implementation
/// @details See @ref SmHalAbstractGpio for details
template <uint32_t GPIO_BASE, uint8_t PIN>
class SmHalGpio: public SmHalAbstractGpio
{
public:

    void setModeSpeed(GpioMode mode, GpioSpeed speed);
    inline void setPin(void)
    {
        ((GPIO_TypeDef *)GPIO_BASE)->BSRR = (1UL << PIN);
    }
    inline void resetPin(void)
    {
        ((GPIO_TypeDef *)GPIO_BASE)->BRR = (1UL << PIN);
    }
    inline uint8_t readPin(void);
};

template <bool lowerPins>
static inline void SetCR(uint32_t gpio, uint8_t pin, uint32_t mode);

template <>
inline void SetCR<true>(uint32_t gpio, uint8_t pin, uint32_t mode)
{
    // Clear config
    ((GPIO_TypeDef *)gpio)->CRL &= ~(0xFUL << (pin << 2));
    // Set config
    ((GPIO_TypeDef *)gpio)->CRL |= (mode << (pin << 2));
}

template <>
inline void SetCR<false>(uint32_t gpio, uint8_t pin, uint32_t mode)
{
    // Clear config
    ((GPIO_TypeDef *)gpio)->CRH &= ~(0xFUL << ((pin - 8) << 2));
    // Set config
    ((GPIO_TypeDef *)gpio)->CRH |= (mode << ((pin - 8) << 2));
}

template <uint32_t GPIO_BASE, uint8_t PIN>
void SmHalGpio<GPIO_BASE, PIN>::setModeSpeed(GpioMode mode, GpioSpeed speed)
{
    uint32_t currentmode = mode & 0x0F;

    // If it's an output pin
    if ((mode & 0x10) != 0)
    {
        /* Output mode - can set speed*/
        currentmode |= speed;
    }
    SetCR< PIN < 8 >(GPIO_BASE, PIN, currentmode);

    // Set pull-up/down
    if (mode == SM_HAL_GPIO_MODE_IN_PD)
    {
        ((GPIO_TypeDef *)GPIO_BASE)->BRR = (1UL << PIN);
    }
    else if (mode == SM_HAL_GPIO_MODE_IN_PU)
    {
        ((GPIO_TypeDef *)GPIO_BASE)->BSRR = (1UL << PIN);
    }
}

template <uint32_t GPIO_BASE, uint8_t PIN>
uint8_t SmHalGpio<GPIO_BASE, PIN>::readPin(void)
{
    if (((GPIO_TypeDef *)GPIO_BASE)->IDR & (1UL << PIN))
        return 1;
    else
        return 0;
}

#endif // SM_HAL_GPIO_H
