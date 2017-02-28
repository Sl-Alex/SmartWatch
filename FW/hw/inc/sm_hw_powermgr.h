#ifndef SM_HW_POWERMGR_H
#define SM_HW_POWERMGR_H

#include <cstdint>
#include "stm32f10x.h"
#include "sm_hal_sys_timer.h"

#define PIN_S1     12UL
#define PIN_S2     3UL
#define PIN_S3     1UL
#define PIN_S4     6UL
#define PIN_BTST   3UL  // BT status
#define PIN_BTRX   10UL // BT RX
#define PIN_AIRQ1  2UL
#define PIN_AIRQ2  1UL
#define PIN_AIRQ3  7UL
#define PIN_ARDY   0UL

#define PIN_ALARM  17UL

/// @brief Power manager interface
/// @details Provides pure virtual callbacks which should be defined in the derived classes
class SmHwPowerMgrIface
{
public:
    SmHwPowerMgrIface(){}
    virtual ~SmHwPowerMgrIface(){}
    virtual void onSleep() = 0;
    virtual void onWake(uint32_t wakeSource) = 0;
};

/// @brief Power manager class
class SmHwPowerMgr: public SmHalSysTimerIface
{
public:
    enum SleepTimeout {
        SM_HW_SLEEP_NONE  = 0,
        SM_HW_SLEEP_SHORT = 2000,
        SM_HW_SLEEP_LONG  = 5000
    };
    enum SleepBlocker {
        SM_HW_SLEEPBLOCKER_DISPLAY      = 0x00000001,
        SM_HW_SLEEPBLOCKER_MENU         = 0x00000002,
        SM_HW_SLEEPBLOCKER_BT           = 0x00000004,
        SM_HW_SLEEPBLOCKER_NOTIFICATION = 0x00000008
   };
    enum WakeSource {
        SM_HW_WAKE_MASK_ALARM     = (1UL << PIN_ALARM),
        SM_HW_WAKE_MASK_KEYBOARD  = ((1UL << PIN_S1) | (1UL << PIN_S2) | (1UL << PIN_S3) | (1UL << PIN_S4)),
        SM_HW_WAKE_MASK_ACCELEROMETER  = (1UL << PIN_AIRQ1),
        SM_HW_WAKE_MASK_ALL = (SM_HW_WAKE_MASK_ALARM |
                              SM_HW_WAKE_MASK_KEYBOARD |
                              SM_HW_WAKE_MASK_ACCELEROMETER)
    };

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
    void blockSleep(SleepBlocker blocker);
    /// @brief Allow device to fall asleep
    void allowSleep(SleepBlocker blocker, uint32_t timeout);

    /// @brief Update power manager state (e.g. sleep timeout)
    void updateState(void);

private:
    struct SmHwPowerMgrSubscriber
    {
        SmHwPowerMgrIface *iface;
    };
    SmHwPowerMgr() {}
    uint8_t mPoolSize;
    SmHwPowerMgrSubscriber *mPool;
    // SmHalSysTimerIface events
    void onTimer(uint32_t timeStamp);

    uint32_t sleepBlockers;
    bool canSleep;
};

#endif // SM_HW_POWERMGR_H
