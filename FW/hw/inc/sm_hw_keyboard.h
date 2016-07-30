#ifndef SM_HW_KEYBOARD_H
#define SM_HW_KEYBOARD_H

#include "sm_hal_sys_timer.h"
#include "sm_hw_powermgr.h"
#include "sm_hal_abstract_gpio.h"

/// @brief Simple GPIO keyboard class
class SmHwKeyboard: public SmHalSysTimerIface, public SmHwPowerMgrIface
{
public:

    /// @brief Constructor
    /// @details Initializes inputs and subscribes for the system timer notifications for debouncing
    /// Subscribes also for the power manager notifications
    SmHwKeyboard();

    /// @brief get button state
    /// @param button: number from 1 to 4
    bool getState(uint8_t button) { return mLastState[button - 1]; }

private:
    const uint8_t DEBOUNCING_TIME = 30;
    void onTimer(uint32_t timeStamp);
    void onSleep(void);
    void onWake(void);
    SmHalAbstractGpio * mGpioPins[4];
    bool mLastState[4];
};

#endif // SM_HW_KEYBOARD_H
