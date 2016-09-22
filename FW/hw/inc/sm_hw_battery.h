#ifndef SM_HW_BATTERY_H
#define SM_HW_BATTERY_H

#include <cstdint>
#include "sm_hal_sys_timer.h"
#ifndef PC_SOFTWARE
#include "sm_hal_abstract_gpio.h"
#include "sm_hw_powermgr.h"

extern "C" void ADC1_2_IRQHandler(void);

#endif


/// @brief Battery status monitoring.
/// @details Monitors battery voltage and charge status.
/// This class is a singleton, because we have only one battery and exclusive ADC access.
/// After \ref init is called, it subscribes to the timer and power manager events.
/// By default, measurement interval is \ref MEAS_INTERVAL.
class SmHwBattery:
    public SmHalSysTimerIface
#ifndef PC_SOFTWARE
    , public SmHwPowerMgrIface
#endif
{
public:
    /// @brief ADC initialization and subscription to the timer and power manager events.
    /// Should be done once after startup.
    void init(void);

    /// @brief Get latest battery value
    /// @todo Implement correct measurement instead of raw ADC data.
    uint32_t getRaw(void) { return mRaw; }
    uint32_t getCharge(void) { return mCharge; }
    uint32_t getVoltage(void) { return mVoltage; }

    /// @brief This class is a singleton, removing
    SmHwBattery(SmHwBattery const&) = delete;
    /// @brief This class is a singleton, removing
    void operator=(SmHwBattery const&) = delete;

    /// @brief Get a singleton instance
    static SmHwBattery* getInstance()
    {
        static SmHwBattery    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return &instance;
    }
private:
    SmHwBattery() {}    ///< Default constructor is hidden

    uint32_t mRaw;      ///< Latest raw value
    uint32_t mCharge;   ///< Latest charge percentage
    bool mChargeStatus; ///< Charge status
    uint32_t mVoltage;  ///< Latest voltage
    uint8_t mMeasStep;  ///< Measurement step (delay/measurement)

#ifndef PC_SOFTWARE
    friend void ADC1_2_IRQHandler(void);

    SmHalAbstractGpio * mGpioEn;    ///< Measurement circuit control pin
    SmHalAbstractGpio * mGpioStatus; ///< Charge status pin
#endif

    // SmHalSysTimerIface events
    void onTimer(uint32_t timeStamp);

#ifndef PC_SOFTWARE
    // SmHalSysTimerIface events
    void onSleep(void);
    void onWake(void);
#endif

    void updateValues(void);
};


#endif // SM_HW_BATTERY_H
