#include <cstring>
#include "sm_hal_sys_timer.h"

SmHalSysTimer::SmHalSysTimerSubscriber *SmHalSysTimer::mPool = 0;
uint8_t SmHalSysTimer::mPoolSize = 0;
uint32_t SmHalSysTimer::mTimeStamp = 0;

bool SmHalSysTimer::subscribe(SmHalSysTimerIface *iface, uint32_t period, bool repeat)
{
    // Search for existing, update if found
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface == iface)
        {
            mPool[i].period = period;
            mPool[i].starttime = mTimeStamp;
            mPool[i].repeat = repeat;
            return true;
        }
    }
    // Subscribe new client
    for (uint32_t i = 0; i < mPoolSize; i++)
    {
        if (mPool[i].iface == 0)
        {
            mPool[i].iface = iface;
            mPool[i].period = period;
            mPool[i].starttime = mTimeStamp;
            mPool[i].repeat = repeat;
            return true;
        }
    }
    // Can not update existing or create new
    return false;
}

void SmHalSysTimer::unsubscribe(SmHalSysTimerIface *iface)
{
    // Search for existing, remove if found
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface == iface)
        {
            mPool[i].iface = 0;
            break;
        }
    }
}

void SmHalSysTimer::initSubscribersPool(uint8_t max)
{
    if (mPool)
        deinitSubscribersPool();

    mPoolSize = max;
    mPool = new SmHalSysTimerSubscriber[mPoolSize];
    memset(mPool, 0, sizeof(SmHalSysTimerSubscriber) * mPoolSize);
}

void SmHalSysTimer::deinitSubscribersPool(void)
{
    if (mPool)
        delete[] mPool;

    mPoolSize = 0;
    mPool = 0;
}

void SmHalSysTimer::init(uint32_t ms)
{
    uint32_t TimerTick = SystemCoreClock/1000UL - 1;
    ms *= TimerTick;

    SysTick_Config(ms);
}

void SmHalSysTimer::processEvents(void)
{
    static uint32_t timeStamp = 0;

    if (mTimeStamp == timeStamp) return;

    timeStamp = mTimeStamp;

    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface != 0)
        {
            uint32_t endtime = mPool[i].starttime + mPool[i].period;
            if (timeStamp >= endtime)
            {
                SmHalSysTimerIface * iface = mPool[i].iface;

                // Clear iface
                if (mPool[i].repeat == false)
                {
                    mPool[i].iface = 0;
                }

                iface->onTimer(timeStamp);

                // Check if we have to stop notifications for this subscriber
                if (mPool[i].repeat)
                {
                    mPool[i].starttime = timeStamp;
                }
            }
        }
    }
}

extern "C" void SysTick_Handler(void)
{
    SmHalSysTimer::mTimeStamp++;
}
