#ifndef SM_HW_KEYBOARD_H
#define SM_HW_KEYBOARD_H

#include "sm_hal_sys_timer.h"
#include "sm_hw_powermgr.h"
#include "sm_hal_abstract_gpio.h"

/// @brief Keyboard interface
/// @details Provides pure virtual callbacks which should be defined in the derived classes
class SmHwKeyboardIface
{
public:
    /// @brief Trivial constructor
    SmHwKeyboardIface(){}
    /// @brief Virtual destructor
    virtual ~SmHwKeyboardIface(){}
    /// @brief onKeyDown event. Should be redefined in the derived class
    virtual void onKeyDown(uint8_t key) = 0;
    /// @brief onKeyUp event. Should be redefined in the derived class
    virtual void onKeyUp(uint8_t key) = 0;
};

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
    bool getState(uint8_t button) { return (mLastState >> button) & 0x01; }

    void initSubscribersPool(uint8_t max);

    void deinitSubscribersPool(void);

    /// @brief Subscribe for the power manager notifications
    bool subscribe(SmHwKeyboardIface *iface);

    /// @brief Unsubscribe for the power manager notifications
    void unsubscribe(SmHwKeyboardIface *iface);

private:
    const uint8_t DEBOUNCING_TIME = 30;
    void onTimer(uint32_t timeStamp);
    void onSleep(void);
    void onWake(void);
    SmHalAbstractGpio * mGpioPins[4];
    uint8_t mLastState;

    struct SmHwKeyboardSubscriber
    {
        SmHwKeyboardIface *iface;
    };
    uint8_t mPoolSize;
    SmHwKeyboardSubscriber *mPool;
};

#endif // SM_HW_KEYBOARD_H
