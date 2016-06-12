#ifndef SM_HAL_SYS_TIMER_H_INCLUDED
#define SM_HAL_SYS_TIMER_H_INCLUDED

#include "stm32f10x.h"

extern "C" void SysTick_Handler(void);

class SmHalSysTimerIface
{
public:
    SmHalSysTimerIface(){}
    virtual ~SmHalSysTimerIface(){}
    virtual void onTimer(uint32_t timeStamp) = 0;
};

struct SmHalSysTimerSubscriber{
    SmHalSysTimerIface *iface;
    uint32_t period;
    uint32_t starttime;
    bool repeat;
};

class SmHalSysTimer
{
public:
    static void init(uint32_t ms);
    static void initSubscribersPool(uint8_t max);
    static void deinitSubscribersPool(void);
    static bool subscribe(SmHalSysTimerIface *iface, uint32_t period, bool repeat);
    static void unsubscribe(SmHalSysTimerIface *iface);
    static void processEvents(void);
private:
    static SmHalSysTimerSubscriber * mPool;
    static uint8_t mPoolSize;
    static uint32_t mTimeStamp;
    friend void SysTick_Handler(void); // We should be able to access timestamp in the IRQ
};

#endif /* SM_HAL_SYS_TIMER_H_INCLUDED */
