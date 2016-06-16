#ifndef SM_HW_BATTERY_H_INCLUDED
#define SM_HW_BATTERY_H_INCLUDED

#include <cstdint>
#include "sm_hal_sys_timer.h"
#include "sm_hal_abstract_gpio.h"
#include "sm_hw_powermgr.h"

extern "C" void ADC1_2_IRQHandler(void);

class SmHwBattery: public SmHalSysTimerIface, public SmHwPowerMgrIface
{
public:
    void init(void);
    uint32_t getValue(void) { return mValue; }
    SmHwBattery(SmHwBattery const&) = delete;
    void operator=(SmHwBattery const&) = delete;
    static SmHwBattery* getInstance()
    {
        static SmHwBattery    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return &instance;
    }
private:
    SmHwBattery() {}
    uint32_t mValue;
    uint8_t mMeasStep;
    friend void ADC1_2_IRQHandler(void);
    SmHalAbstractGpio * mGpioEn;

    // SmHalSysTimerIface
    void onTimer(uint32_t timeStamp);

    // SmHalSysTimerIface
    void onSleep(void);
    void onWake(void);
};


#endif /* SM_HW_BATTERY_H_INCLUDED */
