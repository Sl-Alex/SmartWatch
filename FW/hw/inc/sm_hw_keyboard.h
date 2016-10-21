#ifndef SM_HW_KEYBOARD_H
#define SM_HW_KEYBOARD_H

#include "sm_hal_sys_timer.h"

#ifndef PC_SOFTWARE
#include "sm_hw_powermgr.h"
#include "sm_hal_abstract_gpio.h"
#endif

enum SmHwButtons
{
    SM_HW_BUTTON_EXIT    = 0x01,
    SM_HW_BUTTON_SELECT  = 0x02,
    SM_HW_BUTTON_UP      = 0x04,
    SM_HW_BUTTON_DOWN    = 0x08,
    SM_HW_BUTTON_VIRT_EXIT = 0x10 ///< Virtual button (can be used by object to inform host object that all tasks are done)
};


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
    virtual void onKeyDown(SmHwButtons key) = 0;
    /// @brief onKeyUp event. Should be redefined in the derived class
    virtual void onKeyUp(SmHwButtons key) = 0;
};

/// @brief Simple GPIO keyboard class
class SmHwKeyboard: public SmHalSysTimerIface
#ifndef PC_SOFTWARE
    , public SmHwPowerMgrIface
#endif
{
public:
    /// @brief get button state
    /// @param button: number from 1 to 4
    bool getState(uint8_t button) { return (mLastState >> button) & 0x01; }

    void initSubscribersPool(uint8_t max);

    void deinitSubscribersPool(void);

    /// @brief Subscribe for the power manager notifications
    bool subscribe(SmHwKeyboardIface *iface);

    /// @brief Unsubscribe for the power manager notifications
    void unsubscribe(SmHwKeyboardIface *iface);

#ifdef PC_SOFTWARE
    uint8_t simulatedState;
#endif

    /// @brief This class is a singleton, removing
    SmHwKeyboard(SmHwKeyboard const&) = delete;
    /// @brief This class is a singleton, removing
    void operator=(SmHwKeyboard const&) = delete;
    ~SmHwKeyboard(){}

    /// @brief Get a singleton instance
    static SmHwKeyboard * getInstance()
    {
        static SmHwKeyboard    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return &instance;
    }

private:
    // Default constructor is hidden
    SmHwKeyboard();

    const uint8_t DEBOUNCING_TIME = 30;
    void onTimer(uint32_t timeStamp);
#ifndef PC_SOFTWARE
    void onSleep(void);
    void onWake(void);
    SmHalAbstractGpio * mGpioPins[4];
#endif
    uint8_t mLastState;

    struct SmHwKeyboardSubscriber
    {
        SmHwKeyboardIface *iface;
    };
    uint8_t mPoolSize;
    SmHwKeyboardSubscriber *mPool;
};

#endif // SM_HW_KEYBOARD_H
