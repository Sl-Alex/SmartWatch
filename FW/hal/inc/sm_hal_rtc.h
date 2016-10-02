#ifndef SM_HAL_RTC_H
#define SM_HAL_RTC_H

#include <cstdint>

#ifndef PC_SOFTWARE
#include "stm32f10x.h"
#endif

//extern "C" void SysTick_Handler(void);

/*
/// @brief System timer interface
/// @details Provides pure virtual callbacks which should be defined in the derived classes
class SmHalRtcIface
{
public:
    /// @brief Trivial constructor
    SmHalSysTimerIface(){}
    /// @brief Virtual destructor
    virtual ~SmHalSysTimerIface(){}
    /// @brief onTimer event. Should be redefined in the derived class
    virtual void onTimer(uint32_t timeStamp) = 0;
};
*/

/// @brief RTC
class SmHalRtc
{
public:
    /// @brief Initialize system timer with given resolution
    static void init();
    static uint32_t getCounter(void);
    static void setCounter(uint32_t value);
private:
//    friend void SysTick_Handler(void); // We should be able to access timestamp in the IRQ
};

#endif // SM_HAL_RTC_H
