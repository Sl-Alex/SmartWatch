#ifndef SM_DISPLAY_H
#define SM_DISPLAY_H

#include "sm_canvas.h"
#include "sm_hal_abstract_gpio.h"
#include "sm_hal_abstract_spi.h"
#include "sm_hw_powermgr.h"

/// @brief Display class
class SmDisplay: public SmHwPowerMgrIface
{
public:
    /// @brief Constructor
    /// @details Initializes display and subscribes to SmHwPowerMgr events
    SmDisplay();

    /// @brief Initializes display buffer (built-in SmCanvas)
    void init(int width, int height);

    /// @brief Output display buffer to the display
    void update(void);

    /// @brief Fill display directly with a pattern (useful for clearing it before enable)
    /// @todo Move it to private section
    void fill(uint8_t data);

    /// @brief Get a pointer to the built-in SmCanvas
    inline SmCanvas * getCanvas(void) { return mCanvas; }

private:
    void sendCommand(uint8_t cmd);
    void sendCommand(uint8_t cmd, uint8_t data);
    void sendData(uint8_t * data, uint8_t size);
    void powerOn(void);
    void powerOff(void);
    void onSleep(void);
    void onWake(void);
    SmCanvas * mCanvas;
    SmHalAbstractSpi * mSpi;
    SmHalAbstractGpio * mDcPin;
    SmHalAbstractGpio * mPowerPin;
    SmHalAbstractGpio * mResetPin;
};

#endif // SM_DISPLAY_H
