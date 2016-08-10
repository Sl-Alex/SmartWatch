#include <cstring>

#include "sm_hw_keyboard.h"
#ifndef PC_SOFTWARE
#include "sm_hal_gpio.h"
#endif

SmHwKeyboard::SmHwKeyboard(void)
{
    mPool = 0;
    mPoolSize = 0;
#ifndef PC_SOFTWARE
    // Construct pins
    mGpioPins[0] = new SmHalGpio<GPIOA_BASE, 12>();
    mGpioPins[1] = new SmHalGpio<GPIOB_BASE, 3>();
    mGpioPins[2] = new SmHalGpio<GPIOA_BASE, 1>();
    mGpioPins[3] = new SmHalGpio<GPIOA_BASE, 6>();

    // Initialize as inputs
    for (uint8_t i = 0; i < 4; ++i)
    {
        mGpioPins[i]->setModeSpeed(SM_HAL_GPIO_MODE_IN_PU, SM_HAL_GPIO_SPEED_2M);
    }
#endif
    mLastState = 0;

    SmHalSysTimer::subscribe(this, DEBOUNCING_TIME, true);
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->subscribe(this);
#endif
}

#ifndef PC_SOFTWARE
void SmHwKeyboard::onSleep(void)
{
    //mLastState = 0;
}

void SmHwKeyboard::onWake(void)
{
    // Real timestamp does not matter in this case;
    onTimer(0);
}
#endif

/// @todo Check subscribers notification
void SmHwKeyboard::onTimer(uint32_t timeStamp)
{
    uint8_t newState = 0;
    // Update values
#ifndef PC_SOFTWARE
    for (uint8_t key = 0; key < 4; ++key)
    {
        newState |= (!mGpioPins[key]->readPin()) << key;
    }
#else
    newState = simulatedState;
#endif
    // Notify only if there are some changes
    uint8_t changes = newState ^ mLastState;
    mLastState = newState;
    if (changes)
    {
        uint8_t bit = 0x01;
        // Check each key
        for (uint8_t key = 0; key < 4; ++key)
        {
            if (changes & bit)
            {
                // Notify each subscriber
                for (uint32_t i = 0; i < mPoolSize; ++i)
                {
                    if (mPool[i].iface)
                    {
                        if (newState & bit)
                        {
                            mPool[i].iface->onKeyDown(key);
                        }
                        else
                        {
                            mPool[i].iface->onKeyUp(key);
                        }
                    }
                }
            }
            bit <<= 1;
        }
    }
}

bool SmHwKeyboard::subscribe(SmHwKeyboardIface *iface)
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

void SmHwKeyboard::initSubscribersPool(uint8_t max)
{
    if (mPool)
        deinitSubscribersPool();

    mPoolSize = max;
    mPool = new SmHwKeyboardSubscriber[mPoolSize];
    memset(mPool, 0, sizeof(SmHwKeyboardSubscriber) * mPoolSize);
}

void SmHwKeyboard::deinitSubscribersPool(void)
{
    if (mPool)
        delete[] mPool;

    mPoolSize = 0;
    mPool = 0;
}
