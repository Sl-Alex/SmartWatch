#ifndef SM_HAL_RTC_H
#define SM_HAL_RTC_H

#include <cstdint>

#ifndef PC_SOFTWARE
#include "stm32f10x.h"
#include "sm_hw_powermgr.h"
#else
#include "emulator_window.h"
#endif

extern "C" void RTCAlarm_IRQHandler(void);

/// @brief RTC
class SmHalRtc
#ifndef PC_SOFTWARE
: public SmHwPowerMgrIface
#endif
{
public:
    struct SmHalRtcTime
    {
        uint32_t year;      ///< Year (i.e. 2014)
        uint32_t month;     ///< Month (1..12)
        uint32_t day;       ///< Day (1..28-31)
        uint32_t hour;      ///< Hour (0..23)
        uint32_t minute;    ///< Minute (0..59)
        uint32_t second;    ///< Second (0..59)
    };

    /// @brief Initialize system timer with given resolution
    void init();
    void setDateTime(SmHalRtcTime &time);
    SmHalRtcTime getDateTime(void);
    void setDate(SmHalRtcTime &time);
    void setTime(SmHalRtcTime &time);

    bool isHm10Preconf(void);
    void setHm10Preconf(void);

    bool isUpdateRequested(void);
    void setUpdateRequested(void);
    void clearUpdateRequested(void);

    /// @brief Calculates day of week
    /// @param[in] year - full year (2014, for example)
    /// @param[in] month - Month number (1..12)
    /// @param[in] day - Day of month (1..31)
    /// @return 0 for Sunday
    /// @return 1..6 for Monday..Saturday
    uint32_t getDayOfWeek(SmHalRtcTime &time);
    /// @brief Is current year leap
    /// @param[in] year - full year (2014, for example)
    /// @return 1 for leap years, 0 otherwise
    uint32_t isLeap(uint32_t year);
    /// @brief Calculate number of days in specified month
    /// @param[in] year - full year (2014, for example)
    /// @param[in] month - Month number (1..12)
    /// @return number of days (28..31)
    uint32_t getDaysOfMonth(uint32_t year, uint32_t month);

    /// @brief This class is a singleton, removing
    SmHalRtc(SmHalRtc const&) = delete;
    /// @brief This class is a singleton, removing
    void operator=(SmHalRtc const&) = delete;

    /// @brief Get a singleton instance
    static SmHalRtc* getInstance()
    {
        static SmHalRtc instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
        return &instance;
    }
private:
    SmHalRtc() {}   ///< Default constructor is hidden

    void setAlarm(uint32_t value);
    uint32_t getAlarm(void);
    void updateAlarm(void);
    uint32_t getCounter(void);
    void setCounter(uint32_t value);

    // Power manager interface
    void onSleep(void);
    void onWake(uint32_t wakeSource);

    friend void RTCAlarm_IRQHandler(void);

#ifdef PC_SOFTWARE
    friend class EmulatorWindow;
    void incrementCounter(void);
#endif
};

#endif // SM_HAL_RTC_H
