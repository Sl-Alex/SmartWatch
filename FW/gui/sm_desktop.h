#ifndef SM_DESKTOP_H
#define SM_DESKTOP_H

#include <cstring>
#include "sm_canvas.h"
#include "sm_font.h"
#include "sm_hw_battery.h"

/// @brief Desktop class
class SmDesktop: public SmHalSysTimerIface
{
public:

    /// @brief Initialize desktop with its default values
    void init(SmCanvas * canvas);

    /// @brief This class is a singleton, removing
    SmDesktop(SmDesktop const&) = delete;
    /// @brief This class is a singleton, removing
    void operator=(SmDesktop const&) = delete;
    ~SmDesktop();

    /// @brief Get a singleton instance
    static SmDesktop* getInstance()
    {
        static SmDesktop    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return &instance;
    }

    /// @brief System timer event
    void onTimer(uint32_t timeStamp);

private:
    static const uint8_t MAX_ICONS_COUNT = 10;
    const uint8_t INVALID_ICON = UINT8_MAX;

    const uint8_t ICON_POS_POWER = MAX_ICONS_COUNT - 1; ///< Last icon
    const uint8_t ICON_POS_BATT = ICON_POS_POWER - 1;   ///< Right before the last
    const uint8_t ICON_POS_BT = ICON_POS_BATT - 1;      ///< Before the battery

    SmDesktop() {}    ///< Default constructor is hidden

    void setIcon(uint8_t pos, uint8_t icon);
    void drawIcons(void);


    /// @brief BLE signal strength
    /// @todo Define values
    uint8_t mSignalStrength;

    /// @brief Battery level
    /// @todo Define values
    uint8_t mBatteryLevel;
    SmHwBattery::BatteryStatus mBatteryStatus;

    SmHwBattery * pBattery;

    SmCanvas * pCanvas;

    uint8_t icons[MAX_ICONS_COUNT];

    SmFont * pFont7SegBig;
    SmFont * pFont7SegSmall;
};

#endif // SM_DESKTOP_H
