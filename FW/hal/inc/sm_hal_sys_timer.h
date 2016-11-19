#ifndef SM_HAL_SYS_TIMER_H
#define SM_HAL_SYS_TIMER_H

#include <cstdint>

#ifdef PC_SOFTWARE
#include "qglobal.h"
#else
#include "stm32f10x.h"
#endif

extern "C" void SysTick_Handler(void);

/// @brief System timer interface
/// @details Provides pure virtual callbacks which should be defined in the derived classes
class SmHalSysTimerIface
{
public:
    /// @brief Trivial constructor
    SmHalSysTimerIface(){}
    /// @brief Virtual destructor
    virtual ~SmHalSysTimerIface(){}
    /// @brief onTimer event. Should be redefined in the derived class
    virtual void onTimer(uint32_t timeStamp) = 0;
};

/// @brief System timer
class SmHalSysTimer
{
public:
    /// @brief Initialize system timer with given resolution
    static void init(uint32_t ms);
    /// @brief Init memory pool for all subscribers
    static void initSubscribersPool(uint8_t max);
    /// @brief Deinit subscribers memory pool
    static void deinitSubscribersPool(void);
    /// @brief Subscribe new timer client or update existing
    static bool subscribe(SmHalSysTimerIface *iface, uint32_t period, bool repeat);
    /// @brief Unsubscribe timer client
    static void unsubscribe(SmHalSysTimerIface *iface);
    /// @brief Process events and notify subscribers if necessary
    static void processEvents(void);
    /// @brief Get current time stamp
    static uint32_t getTimeStamp(void) { return mTimeStamp; }
private:
    struct SmHalSysTimerSubscriber{
        SmHalSysTimerIface *iface;
        uint32_t period;
        uint32_t starttime;
        bool repeat;
    };
    static SmHalSysTimerSubscriber * mPool;
    static uint8_t mPoolSize;
    volatile static uint32_t mTimeStamp;

    friend void SysTick_Handler(void); // We should be able to access timestamp in the IRQ
};

#endif // SM_HAL_SYS_TIMER_H
