#ifndef SM_DESKTOP_H
#define SM_DESKTOP_H

#include <cstring>
#include "sm_canvas.h"
#include "sm_hw_battery.h"

/// @brief Desktop class
class SmDesktop: public SmHalSysTimerIface
{
public:

    /// @brief Initialize desktop with its default values
    void init(void);

    /// @brief This class is a singleton, removing
    SmDesktop(SmDesktop const&) = delete;
    /// @brief This class is a singleton, removing
    void operator=(SmDesktop const&) = delete;

    /// @brief Get a singleton instance
    static SmDesktop* getInstances()
    {
        static SmDesktop    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return &instance;
    }

    /// @brief System timer event
    void onTimer(uint32_t timeStamp);

private:
    SmDesktop() {}    ///< Default constructor is hidden

    /// @brief BLE signal strength
    /// @todo Define values
    uint8_t mSignalStrength;

    /// @brief Battery level
    /// @todo Define values
    uint8_t mBatteryLevel;

    SmHwBattery * pBattery;
};

#endif // SM_DESKTOP_H