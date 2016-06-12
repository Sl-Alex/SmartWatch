#ifndef SM_HW_BATTERY_H_INCLUDED
#define SM_HW_BATTERY_H_INCLUDED

#include <cstdint>
#include "sm_hal_sys_timer.h"
#include "sm_hal_abstract_gpio.h"

extern "C" void ADC1_2_IRQHandler(void);

class SmHwBattery: public SmHalSysTimerIface
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
    uint32_t readValue(void);
    uint32_t mValue;
    void onTimer(uint32_t timeStamp);
    friend void ADC1_2_IRQHandler(void);
    SmHalAbstractGpio * mGpioEn;
};


#endif /* SM_HW_BATTERY_H_INCLUDED */
