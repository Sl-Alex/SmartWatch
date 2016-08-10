#ifndef SM_HW_MOTOR_H
#define SM_HW_MOTOR_H

#include "sm_hal_abstract_gpio.h"
#include "sm_hal_sys_timer.h"
#include "sm_hw_powermgr.h"
#include "sm_hw_keyboard.h"

/// @brief Vibration motor control
class SmHwMotor: public SmHalSysTimerIface, public SmHwPowerMgrIface, public SmHwKeyboardIface
{
public:
    /// @brief Initialize motor control GPIO and subscribe to the system timer events
    SmHwMotor();
private:
    void onTimer(uint32_t timeStamp);
    void onSleep(void);
    void onWake(void);

    /// @todo Remove after testing
    void onKeyDown(uint8_t key);
    /// @todo Remove after testing
    void onKeyUp(uint8_t key);

    SmHalAbstractGpio * mGpio;
};

#endif // SM_HW_MOTOR_H
