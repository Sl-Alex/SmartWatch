#include <cstring>
#include "sm_hw_powermgr.h"

#define PWR_Regulator_ON          ((uint32_t)0x00000000)
#define PWR_Regulator_LowPower    ((uint32_t)0x00000001)

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)

/// @todo Implement
void SmHwPowerMgr::init(void)
{
    mPool = 0;
    mPoolSize = 0;

    // 1. Init EXTI for all possible WFE sources
    // (4 buttons, bt status, ACC/MAG IRQs)
}

void SmHwPowerMgr::initSubscribersPool(uint8_t max)
{
    if (mPool)
        deinitSubscribersPool();

    mPoolSize = max;
    mPool = new SmHwPowerMgrSubscriber[mPoolSize];
    memset(mPool, 0, sizeof(SmHwPowerMgrSubscriber) * mPoolSize);
}

void SmHwPowerMgr::deinitSubscribersPool(void)
{
    if (mPool)
        delete[] mPool;

    mPoolSize = 0;
    mPool = 0;
}

bool SmHwPowerMgr::subscribe(SmHwPowerMgrIface *iface, uint32_t period, bool repeat)
{
    // Search for existing, update if found
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface == iface)
        {
            return true;
        }
    }
    // Subscribe new client
    for (uint32_t i = 0; i < mPoolSize; i++)
    {
        if (mPool[i].iface == 0)
        {
            mPool[i].iface = iface;
            return true;
        }
    }
    // Can not update existing or create new
    return false;
}

void SmHwPowerMgr::unsubscribe(SmHwPowerMgrIface *iface)
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

/// @todo Implement subscribers notification on sleep and on wake
void SmHwPowerMgr::sleep(void)
{
    // Call onSleep() for all subscribers
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface != 0)
        {
            mPool[i].iface->onSleep();
        }
    }

    // Entering STOP state with low power regulator mode and WFE
    uint32_t tmpreg = 0;

    // Select the regulator state in STOP mode
    tmpreg = PWR->CR;
    // Clear PDDS and LPDS bits
    tmpreg &= CR_DS_MASK;
    // Set LPDS bit according to PWR_Regulator value
    tmpreg |= PWR_Regulator_LowPower;
    // Store the new value
    PWR->CR = tmpreg;
    // Set SLEEPDEEP bit of Cortex System Control Register
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    // Goes to sleep at this step
    __WFE();

    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);

    // Call onWake() for all subscribers
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface != 0)
        {
            mPool[i].iface->onWake();
        }
    }
}
