#ifndef SM_HW_MOTOR_H
#define SM_HW_MOTOR_H

#include "sm_hal_abstract_gpio.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_powermgr.h"
#include "sm_hw_keyboard.h"

/// @brief Vibration motor control
class SmHwMotor: public SmHalSysTimerIface, public SmHwPowerMgrIface
{
public:
    /// @brief Initialize motor control GPIO and subscribe to the system timer events
    SmHwMotor();
    void startNotification(uint32_t pattern, uint8_t pulses);
    void stopNotification(void);
private:
    void onTimer(uint32_t timeStamp);
    void onSleep(void);
    void onWake(uint32_t wakeSource);

    SmHalAbstractGpio * mGpio;

    inline void enable(void) { mGpio->setPin(); }
    inline void disable(void) { mGpio->resetPin(); }

    uint32_t mPattern;
    uint32_t mLength;
};

#endif // SM_HW_MOTOR_H
