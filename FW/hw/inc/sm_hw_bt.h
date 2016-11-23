#ifndef SM_HW_BT_H
#define SM_HW_BT_H

#include <cstdint>
#ifndef PC_SOFTWARE
#include "sm_hal_abstract_gpio.h"
#else
#include "emulator_window.h"
#endif

struct __attribute__((__packed__)) SmHwBtPacket {
    uint32_t crc32;
    char data[16];
};

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
    /// @brief Read status pin
    bool isConnected(void);
    /// @brief Run mRxPacket parser, acknowledge generation and so on
    void update(void);

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

    /// @brief Send a prepared packet
    void send(void);

    SmHwBtPacket mRxPacket;
    SmHwBtPacket mTxPacket;
    bool mRxDone;
#ifndef PC_SOFTWARE
    SmHalAbstractGpio *mPowerPin;
    SmHalAbstractGpio *mRxPin;
    SmHalAbstractGpio *mTxPin;
    SmHalAbstractGpio *mStPin;
#else
    /// @brief Inject a packet from another class
    void injectPacket(char * data, uint8_t size);
#endif
    friend class EmulatorWindow;
};

#endif // SM_HW_BT_H
