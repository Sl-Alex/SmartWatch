#ifndef SM_DESKTOP_H
#define SM_DESKTOP_H

#include <cstring>
#include "sm_canvas.h"
#include "sm_font.h"
#include "sm_hw_battery.h"
#include "sm_hw_keyboard.h"
#include "sm_main_menu.h"
#include "sm_notification.h"

#define DIGITS_COUNT 6

/// @brief Desktop class
class SmDesktop: public SmHalSysTimerIface, public SmHwKeyboardIface
#ifndef PC_SOFTWARE
    , public SmHwPowerMgrIface
#endif
{
public:

    /// @brief Initialize desktop with its default values
    void init(void);

    /// @brief This class is a singleton, removing
    SmDesktop(SmDesktop const&) = delete;
    /// @brief This class is a singleton, removing
    void operator=(SmDesktop const&) = delete;
    ~SmDesktop();

    const char * getVersion(void);

    /// @brief Get a singleton instance
    static SmDesktop* getInstance()
    {
        static SmDesktop    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return &instance;
    }

    /// @brief System timer event
    void onTimer(uint32_t timeStamp);

    /// @brief onKeyDown event.
    void onKeyDown(SmHwButtons key);
    /// @brief onKeyUp event.
    void onKeyUp(SmHwButtons key);

    void showNotification(SmText header, SmText text);
    inline SmNotification * getNotification(void) { return pNotification; }

private:
    static const uint8_t MAX_ICONS_COUNT = 10;
    const uint8_t INVALID_ICON = UINT8_MAX;

    const uint8_t ICON_POS_POWER = MAX_ICONS_COUNT - 1; ///< Last icon
    const uint8_t ICON_POS_BATT = ICON_POS_POWER - 1;   ///< Right before the last
    const uint8_t ICON_POS_BT = ICON_POS_BATT - 1;      ///< Before the battery

    SmDesktop() {}    ///< Default constructor is hidden

    void setIcon(uint8_t pos, uint8_t icon);
    void drawIcons(void);
    void drawAll(void);
    void drawTime(void);

    void onSleep(void);
    void onWake(uint32_t WakeSource);


    /// @brief BLE signal strength
    /// @todo Define values
    uint8_t mSignalStrength;

    /// @brief Battery level
    uint8_t mBatteryLevel;
    SmHwBattery::BatteryStatus mBatteryStatus;

    SmHwBattery * pBattery;

    SmCanvas * pCanvas;

    uint8_t icons[MAX_ICONS_COUNT];

    SmFont * pFont7SegBig;
    SmFont * pFont7SegSmall;
    SmFont * pFontSmall;
    SmMainMenu * pMainMenu;
    SmNotification * pNotification;

    SmAnimator menuAnimator;

    SmAnimator mDigitAnimators[DIGITS_COUNT];
    SmCanvas mNewDigits[DIGITS_COUNT];
    uint8_t mDigitsOffsetX[DIGITS_COUNT];
    uint8_t mDigitsOffsetY[DIGITS_COUNT];

    uint8_t mDigits[DIGITS_COUNT];
};

#endif // SM_DESKTOP_H
