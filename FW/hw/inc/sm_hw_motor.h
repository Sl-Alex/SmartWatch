#ifndef SM_HW_MOTOR_H_INCLUDED
#define SM_HW_MOTOR_H_INCLUDED

#include "sm_hal_abstract_gpio.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_powermgr.h"

/// @brief Vibration motor control
class SmHwMotor: public SmHalSysTimerIface, public SmHwPowerMgrIface
{
public:
    /// @brief Initialize motor control GPIO and subscribe to the system timer events
    SmHwMotor();
private:
    void onTimer(uint32_t timeStamp);
    void onSleep(void);
    void onWake(void);
    SmHalAbstractGpio * mGpio;
};

#endif /* SM_HW_MOTOR_H_INCLUDED */
