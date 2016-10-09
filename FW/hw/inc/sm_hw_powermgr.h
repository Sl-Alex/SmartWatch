#ifndef SM_HW_POWERMGR_H
#define SM_HW_POWERMGR_H

#include <cstdint>
#include "stm32f10x.h"
#include "sm_hal_sys_timer.h"

/// @brief Power manager interface
/// @details Provides pure virtual callbacks which should be defined in the derived classes
class SmHwPowerMgrIface
{
public:
    SmHwPowerMgrIface(){}
    virtual ~SmHwPowerMgrIface(){}
    virtual void onSleep() = 0;
    virtual void onWake() = 0;
};

/// @brief Power manager class
class SmHwPowerMgr: public SmHalSysTimerIface
{
public:
    /// @brief Initialize wakeup lines
    void init(void);

    /// @brief Call it when you want switch to the sleep mode
    void sleep(void);

    SmHwPowerMgr(SmHwPowerMgr const&) = delete;
    void operator=(SmHwPowerMgr const&) = delete;
    static SmHwPowerMgr* getInstance()
    {
        static SmHwPowerMgr  instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
        return &instance;
    }

    /// @brief Initialize subscribers pool
    void initSubscribersPool(uint8_t max);

    /// @brief Deinitialize subscribers pool
    void deinitSubscribersPool(void);

    /// @brief Subscribe for the power manager notifications
    bool subscribe(SmHwPowerMgrIface *iface);

    /// @brief Unsubscribe for the power manager notifications
    void unsubscribe(SmHwPowerMgrIface *iface);

    /// @brief Block device from falling asleep
    void blockSleep(SmHwPowerMgrIface * iface);
    /// @brief Allow device to falling asleep
    void unblockSleep(SmHwPowerMgrIface * iface);

    /// @brief Update power manager state (e.g. sleep timeout)
    void updateState(void);

private:
    struct SmHwPowerMgrSubscriber
    {
        SmHwPowerMgrIface *iface;
        bool canSleep;
    };
    SmHwPowerMgr() {}
    uint8_t mPoolSize;
    SmHwPowerMgrSubscriber *mPool;
    // SmHalSysTimerIface events
    void onTimer(uint32_t timeStamp);
};

#endif // SM_HW_POWERMGR_H
