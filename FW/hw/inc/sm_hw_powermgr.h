#ifndef SM_HW_POWERMGR_H_INCLUDED
#define SM_HW_POWERMGR_H_INCLUDED

#include <cstdint>
#include "stm32f10x.h"

class SmHwPowerMgrIface
{
public:
    SmHwPowerMgrIface(){}
    virtual ~SmHwPowerMgrIface(){}
    virtual void onSleep() = 0;
    virtual void onWake() = 0;
};

struct SmHwPowerMgrSubscriber
{
    SmHwPowerMgrIface *iface;
};

class SmHwPowerMgr
{
public:
    void init(void);
    void sleep(void);
    SmHwPowerMgr(SmHwPowerMgr const&) = delete;
    void operator=(SmHwPowerMgr const&) = delete;
    static SmHwPowerMgr* getInstance()
    {
        static SmHwPowerMgr  instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
        return &instance;
    }
    void initSubscribersPool(uint8_t max);
    void deinitSubscribersPool(void);
    bool subscribe(SmHwPowerMgrIface *iface);
    void unsubscribe(SmHwPowerMgrIface *iface);
private:
    SmHwPowerMgr() {}
    uint8_t mPoolSize;
    SmHwPowerMgrSubscriber *mPool;
};

#endif /* SM_HW_POWERMGR_H_INCLUDED */
