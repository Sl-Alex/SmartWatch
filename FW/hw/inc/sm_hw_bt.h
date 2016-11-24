#ifndef SM_HW_BT_H
#define SM_HW_BT_H

#include <cstdint>
#ifndef PC_SOFTWARE
#include "sm_hal_abstract_gpio.h"
#else
#include "emulator_window.h"
#endif

/// @brief BLE packet size (must be less than 20)
#define SM_HW_BT_PACKET_SIZE 20

//extern SmHwBtPacket;
struct SmHwBtPacket;

#define SM_HW_BT_PACKET_HEADER_SIZE (5)
#define SM_HW_BT_PACKET_CONTENT_SIZE (SM_HW_BT_PACKET_SIZE - SM_HW_BT_PACKET_HEADER_SIZE)

struct __attribute__((__packed__)) SmHwBtPacket
{
    struct __attribute__((__packed__)) {
        uint32_t crc32;
        uint8_t type;
    } header;
    union __attribute__((__packed__))
    {
        /// Raw content access
        char raw[SM_HW_BT_PACKET_CONTENT_SIZE];

        /// Only for a SM_HW_BT_PACKET_ACK response
        struct __attribute__((__packed__))
        {
            uint8_t type;       ///< Which packet type is acknowledged
            uint16_t seqNumber; ///< Which seqNumber is acknowledged (if applicable)
        } ack;

        /// Only for a SM_HW_BT_PACKET_VERSION response
        char version[SM_HW_BT_PACKET_CONTENT_SIZE];

        /// Only for a SM_HW_BT_PACKET_NOTIFICATION_HEADER request
        struct __attribute__((__packed__))
        {
            uint32_t size;      ///< Notification size (bytes)
        } notification_header;

        /// Only for a SM_HW_BT_PACKET_NOTIFICATION_DATA request
        struct __attribute__((__packed__))
        {
            uint16_t seqNumber; ///< Sequence number, starting from 0
            char data[SM_HW_BT_PACKET_CONTENT_SIZE - sizeof(uint16_t)]; ///< Notification data
        } notification;

        /// Only for a SM_HW_BT_PACKET_UPDATE_HEADER request
        struct __attribute__((__packed__))
        {
            uint32_t size;      ///< Update size (bytes)
        } update_header;

        /// Only for a SM_HW_BT_PACKET_UPDATE_DATA request
        struct __attribute__((__packed__))
        {
            uint16_t seqNumber; ///< Sequence number. starting from 0
            char data[SM_HW_BT_PACKET_CONTENT_SIZE - sizeof(uint16_t)]; ///< Update data
        } update;

        /// Only for a SM_HW_BT_PACKET_DATETIME request
        struct __attribute__((__packed__))
        {
            uint16_t year;
            uint8_t month;
            uint8_t day;
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        } datetime;
    } content;
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
