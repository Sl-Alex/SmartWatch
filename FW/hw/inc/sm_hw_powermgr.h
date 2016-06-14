#ifndef SM_HW_POWERMGR_H_INCLUDED
#define SM_HW_POWERMGR_H_INCLUDED

#include <cstdint>

class SmHwPowerMgrIface
{
public:
    SmHwPowerMgrIface(){}
    virtual ~SmHwPowerMgrIface(){}
    virtual void onSleep() = 0;
    virtual void onWake() = 0;
};

class SmHwPowerMgr
{
public:
    void init(void);
    SmHwPowerMgr(SmHwPowerMgr const&) = delete;
    void operator=(SmHwPowerMgr const&) = delete;
    static SmHwPowerMgr* getInstance()
    {
        static SmHwPowerMgr  instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
        return &instance;
    }
private:
    SmHwPowerMgr() {}
};

#endif /* SM_HW_POWERMGR_H_INCLUDED */
