#ifndef SM_DISPLAY_H
#define SM_DISPLAY_H

#include "sm_canvas.h"
#ifndef PC_SOFTWARE
#include "sm_hal_abstract_gpio.h"
#include "sm_hal_abstract_spi.h"
#include "sm_hw_powermgr.h"
#endif

/// @brief Display class
class SmDisplay
#ifndef PC_SOFTWARE
    : public SmHwPowerMgrIface
#endif
{
public:
    /// @brief Initializes display buffer (built-in SmCanvas)
    void init(int width, int height);

    /// @brief Output display buffer to the display
    void update(void);

    /// @brief Get a pointer to the built-in SmCanvas
    inline SmCanvas * getCanvas(void) { return mCanvas; }

    /// @brief Turn power on and update display
    void powerOn(void);
    /// @brief Turn power off
    void powerOff(void);

    /// @brief This class is a singleton, removing
    SmDisplay(SmDisplay const&) = delete;
    /// @brief This class is a singleton, removing
    void operator=(SmDisplay const&) = delete;
    ~SmDisplay(){}

    /// @brief Get a singleton instance
    static SmDisplay* getInstance()
    {
        static SmDisplay    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return &instance;
    }

private:
    SmDisplay();    ///< Default constructor is hidden

    void sendCommand(uint8_t cmd);
    void sendCommand(uint8_t cmd, uint8_t data);
    void sendData(uint8_t * data, uint8_t size);
    void onSleep(void);
    void onWake(uint32_t WakeSource);
    SmCanvas * mCanvas;
#ifndef PC_SOFTWARE
    SmHalAbstractSpi * mSpi;
    SmHalAbstractGpio * mDcPin;
    SmHalAbstractGpio * mPowerPin;
    SmHalAbstractGpio * mResetPin;
#endif
    volatile bool mPowered;
};

#endif // SM_DISPLAY_H
