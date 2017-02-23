#ifndef SM_HW_BT_H
#define SM_HW_BT_H

#include <cstdint>
#ifndef PC_SOFTWARE
#include "sm_hal_abstract_gpio.h"
#else
#include "emulator_window.h"
#endif

/** @file
 *
 * @page BLUETOOTH_COMMUNICATION Bluetooth communication
 *
 * @{
 *
 * @par Description
 *
 * Project uses HM-10 BLE module (BLE <==> UART bridge).
 * Communication between the smartphone (or PC) and the SmartWatch is packet-based.
 * Each request should be acknowledged, either with a data, or at least with empty acknowledge packet
 * Packed is received by SmartWatch byte by byte. If time between two bytes is more than @ref SM_HW_BT_RX_TIMEOUT
 * then previous bytes will be discarded.
 *
 * @par Packet structure
 *
 * @dot
 * digraph packetstruct {node1 [label = "CRC32 (4 bytes) | Type (1 byte) | Content data (15 bytes)", shape = record];}
 * @enddot
 *
 * HM-10 transmits UART data in blocks, up to 20 bytes each, so 20 bytes was selected as a maximum
 * packet size. Packet can be splitted in two parts: header and content.
 * Header contains CRC32 and packet type, data varies depending on a packet type.
 * Packet size is _always_ 20 bytes, unused bytes can contain random values.
 * It is planned to use AES-256 encryption for all packets. Since block size for AES-256 is 128 bits, only last 16 bytes will be encrypted.
 * Four CRC32 bytes remain unencrypted, so it is required to check other packet fields after decryption.
 * Following packet types are implemented:
 *
 * Type                                     | Description
 * -----------------------------------------|------------
 * @ref SM_HW_BT_PACKET_ACK                 | Acknowledge to any packet except of @ref SM_HW_BT_PACKET_VERSION
 * @ref SM_HW_BT_PACKET_VERSION             | Version request and response
 * @ref SM_HW_BT_PACKET_NOTIFICATION_HEADER | Text notification header, containing the length of the notification
 * @ref SM_HW_BT_PACKET_NOTIFICATION_DATA   | Text notification data
 * @ref SM_HW_BT_PACKET_UPDATE_HEADER       | Firmware update header, contains the size of the update
 * @ref SM_HW_BT_PACKET_UPDATE_DATA         | Firmware update data.
 * @ref SM_HW_BT_PACKET_DATETIME            | Set date/time request, contains both date and time
 *
 * Now a bit more details about each type:
 *
 * @par SM_HW_BT_PACKET_ACK
 *
 * This is a response to any request packet. It must contain a type and a sequence number of the request packet. If request packet does not contain a sequence number, then this field must be 0xFF
 *
 * @par SM_HW_BT_PACKET_VERSION
 *
 * This type of packet requests version information (ASCII, NULL-terminated) from the SmartWatch. Request data is not important. Response must have @ref SM_HW_BT_PACKET_ACK type and correct version field.
 * Version is a NULL-terminated ASCII-encoded version string.
 *
 * @par SM_HW_BT_PACKET_NOTIFICATION_HEADER
 *
 * This is the notification request. It is followed by the variable number of the @ref SM_HW_BT_PACKET_NOTIFICATION_DATA packets. It contains notification size (bytes).
 *
 * @par SM_HW_BT_PACKET_NOTIFICATION_DATA
 *
 * This packet contains a portion of the notification data. It must have a sequence number, so the notification could be correctly assembled. Sequence number starts from zero
 * and must be incremented with each new portion of data.
 *
 * @par SM_HW_BT_PACKET_UPDATE_HEADER
 *
 * This is the firmware update request. It is followed by the variable number of the @ref SM_HW_BT_PACKET_UPDATE_DATA packets. It contains firmware size (bytes).
 *
 * @par SM_HW_BT_PACKET_UPDATE_DATA
 *
 * This packet contains a portion of the new firmware update data. It must have a sequence number, so the firmware could be correctly assembled. Sequence number starts from zero
 * and must be incremented with each new portion of data.
 *
 * @par SM_HW_BT_PACKET_DATETIME
 *
 * This packet synchronizes local time on a SmartWatch with the remote time (from Android or PC). It contains year, month, day, hours (in a 24-hour format), minutes and seconds.
 *
 */

/// @brief BLE packet size (must be less than 20)
#define SM_HW_BT_PACKET_SIZE 20

/// @brief BLE receive timeout, milliseconds
#define SM_HW_BT_RX_TIMEOUT  100

#define SM_HW_BT_PACKET_HEADER_SIZE (5)
#define SM_HW_BT_PACKET_CONTENT_SIZE (SM_HW_BT_PACKET_SIZE - SM_HW_BT_PACKET_HEADER_SIZE)

/// @brief BLE packet definitions
enum SmHwBtPacketType
{
    SM_HW_BT_PACKET_ACK = 1,             ///< Acknowledge to any packet except of SM_HW_BT_PACKET_VERSION
    SM_HW_BT_PACKET_VERSION,             ///< Version request
    SM_HW_BT_PACKET_NOTIFICATION_HEADER, ///< Text notification header, containing the length of the notification
    SM_HW_BT_PACKET_NOTIFICATION_DATA,   ///< Text notification data
    SM_HW_BT_PACKET_UPDATE_HEADER,       ///< Firmware update header, contains the size of the update
    SM_HW_BT_PACKET_UPDATE_DATA,         ///< Firmware update data.
    SM_HW_BT_PACKET_DATETIME,            ///< Set date/time request, contains both date and time
    //======================
    SM_HW_BT_PACKET_TYPE_MAX             ///< This element should be always kept last
};

struct __attribute__((__packed__)) SmHwBtPacket
{
    struct __attribute__((__packed__)) {
        uint32_t crc32;
        uint8_t type;
    } header;
    union __attribute__((__packed__))
    {
        /// Raw content access
        uint8_t raw[SM_HW_BT_PACKET_CONTENT_SIZE];

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
    /// @brief Do a BLE module pre-configuration (init baudrate and so on)
    void preConf(void);
    /// @brief Enable UART transmitter and BT power
    void enable(void);
    /// @brief Disable UART transmitter and BT power
    void disable(void);
    /// @brief Read status pin
    bool isConnected(void);
    /// @brief Run mRxPacket parser, acknowledge generation and so on
    void update(void);

    /// @brief Send a prepared packet
    /// @todo Make it private
    void send(void);

    SmHwBt(SmHwBt const&) = delete;
    void operator=(SmHwBt const&) = delete;
    static SmHwBt* getInstance()
    {
        static SmHwBt  instance; // Guaranteed to be destroyed.
                                 // Instantiated on first use.
        return &instance;
    }

    bool isNotification(void);
    void clearNotification(void);
    void getHeader(uint16_t** ppHeader, uint16_t* pHeaderSize);
    void getText(uint16_t** ppText, uint16_t* pTextSize );
private:
    SmHwBt() {}

    void setBaud(uint32_t baud);


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

    uint16_t * mHeader;
    uint16_t * mText;
    uint16_t mHeaderSize;
    uint16_t mTextSize;
};

#endif // SM_HW_BT_H
