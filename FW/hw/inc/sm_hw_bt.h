#ifndef SM_HW_BT_H
#define SM_HW_BT_H

#include <cstdint>
#include "sm_hal_abstract_gpio.h"

/// @brief HM-10 BT module wrapper class
class SmHwBt
{
public:
    /// @brief Init HW
    void init(void);
    /// @brief Enable UART transmitter and BT power
    void enable(void);
    /// @brief Disable UART transmitter and BT power
    void disable(void);
    /// @brief Send a single byte of data
    void send(uint8_t data);
    /// @brief Read status pin
    bool isConnected(void);

    SmHwBt(SmHwBt const&) = delete;
    void operator=(SmHwBt const&) = delete;
    static SmHwBt* getInstance()
    {
        static SmHwBt  instance; // Guaranteed to be destroyed.
                                 // Instantiated on first use.
        return &instance;
    }
private:
    SmHwBt() {}
    SmHalAbstractGpio *mPowerPin;
    SmHalAbstractGpio *mRxPin;
    SmHalAbstractGpio *mTxPin;
    SmHalAbstractGpio *mStPin;
};

#endif // SM_HW_BT_H
