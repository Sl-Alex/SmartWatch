#ifndef SM_HW_SYS_TIMER_H_INCLUDED
#define SM_HW_SYS_TIMER_H_INCLUDED

#include "stm32f10x.h"

class SmHwSysTimerIface
{
public:
    SmHwSysTimerIface(){}
    virtual ~SmHwSysTimerIface(){}
    virtual void onTimer(uint32_t timeStamp) = 0;
};

class SmHwSysTimer
{
public:
    static bool subscribe(SmHwSysTimerIface *iface, uint32_t period, bool repeat);
    static void unsubscribe(SmHwSysTimerIface *iface);
private:
    /// @todo Implement subscribers pool
};

#endif /* SM_HW_SYS_TIMER_H_INCLUDED */
