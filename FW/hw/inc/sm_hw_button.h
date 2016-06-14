#ifndef SM_HW_BUTTON_H_INCLUDED
#define SM_HW_BUTTON_H_INCLUDED

#include <cstdint>
#include "sm_hal_abstract_gpio.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_powermgr.h"

class SmHwButton: public SmHalSysTimerIface, public SmHwPowerMgrIface
{
public:
    void init(SmHalAbstractGpio * pin);
    bool getState(void) { return mLastState; }
private:
    const uint8_t DEBOUNCING_TIME = 30;
    void onTimer(uint32_t timeStamp);
    void onSleep(void) {};
    void onWake(void) { mLastState = false; };
    SmHalAbstractGpio * mGpio;
    bool mLastState;
};

#endif /* SM_HW_BUTTON_H_INCLUDED */
