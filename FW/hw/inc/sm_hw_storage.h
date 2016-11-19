#ifndef SM_HW_STORAGE_H
#define SM_HW_STORAGE_H

#include <cstdint>

#ifndef PC_SOFTWARE
#include "sm_hal_abstract_spi.h"
#include "sm_hw_powermgr.h"
#else
#include <fstream>
#endif

/** @defgroup EXTERNAL_FLASH External flash
 *
 * @{
 *
 * @par Requirements
 *
 * External SPI flash memory holds FW update, fonts, images and so on.
 * Microcontroller does not have such a big amount of the non-volatile memory.
 * External SPI flash is fast enough and has enough space to store all this stuff.
 *
 * @par Memory layout
 *
 * Before using this memory we have to define its layout, to be sure that everything
 * is stored exactly where it should be. A kind of a very simple file system table is
 * located in the very beginning of the flash. Actually, it contains just a number of
 * elements, their parameters (offset and size in bytes) and CRC32 in the end.
 *
 * The whole table is verified during the start-up.
 * Access to any element is blocked if CRC32 is wrong.
 *
 * Offset       | Parameter | Size  | Description
 * ------------ | --------- | ----- | ------
 * 0x0000       | nElements | 8     | Number of elements stored
 * 0x0001       | offset1   | 32    | Offset of the first element. Must be equal to 1 + nElements*8 + 4.
 * 0x0005       | size1     | 32    | Size of the first element
 * 0x0009       | offset2   | 32    | Offset of the second element
 * 0x000D       | size2     | 32    | Size of the second element
 * ...          | ...       | ...   | ... Other parameters ...
 * offset1 - 4  | crc32     | 32    | Checksum of the header
 * offset1      | data1     | size1 | Actual data for the first element
 * offset2      | data2     | size2 | Actual data for the second element
 * ...          | ...       | ...   | ... Other data elements ...
 *
 */

/// @brief HW storage element information
struct SmHwStorageElementInfo
{
    uint32_t offset;    ///< Element offset (bytes)
    uint32_t size;      ///< Element size (bytes)
};

struct SmHwStorageSettings
{
    uint32_t BtPin;
};

#include "sm_strings.h"

/// @brief HW storage elements indices
enum SmHwStorageIndices {
    // Fixed part begin
    IDX_FW_VERSION = 0,     ///< FW version
    IDX_FW_UPDATE,          ///< FW itself
    // Fixed part end, the rest is FW-dependent
    IDX_FW_FONT_SMALL,      ///< Small font
    IDX_FW_FONT_1_LARGE,    ///< Big clock font
    IDX_FW_FONT_1_MEDIUM,   ///< Small clock font
    IDX_FW_FONT_2_LARGE,    ///< Big clock font
    IDX_FW_FONT_2_MEDIUM,   ///< Small clock font
    IDX_ICON_BLUETOOTH,
    IDX_ICON_DATE,
    IDX_ICON_TIME,
    IDX_ICON_ARROW_DOWN,
    IDX_ICON_ARROW_UP
};

/// @brief External SPI flash class
class SmHwStorage
#ifndef PC_SOFTWARE
    : public SmHwPowerMgrIface
#endif
{
public:
    /// @brief Initialize HW access, verify checksum
    void init(void);

    SmHwStorage(SmHwStorage const&) = delete;
    void operator=(SmHwStorage const&) = delete;
    static SmHwStorage* getInstance()
    {
        static SmHwStorage    instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return &instance;
    }

    #ifndef PC_SOFTWARE
    /// @brief Read external flash ID
    uint32_t readId(void);
    #endif

    /// @brief Read element size
    /// @param element: Element index
    /// @return Element size
    uint32_t getElementSize(uint8_t element);

    /// @brief get element info
    bool getElementInfo(uint8_t element, SmHwStorageElementInfo * info);

    /// @brief Read element or its part
    /// @param element: Element index
    /// @param offset: Offset from the element start
    /// @param pData[out]: Where to put the result
    /// @param size: Number of bytes to read. The whole element will be read if zero
    void readElement(uint8_t element, uint32_t offset, uint8_t * pData, uint32_t size = 0);

    /// @brief Read arbitrary data
    /// @param offset: Offset from the flash start
    /// @param pData[out]: Where to put the result
    /// @param size: Number of bytes to read
    void readData(uint32_t offset, uint8_t *pData, uint32_t size);

    /// @brief Read user settings structure
    void readSettings(SmHwStorageSettings * settings);
    /// @brief Write user settings structure
    void writeSettings(const SmHwStorageSettings * const settings);

    /// @brief Get elements count
    inline uint32_t elementsCount(void) { return mCount; }

    void startFwDataErase(void);
    bool isFwDataErased(void);

    void startSettingsErase(void);
    bool isSettingsErased(void);
private:
    SmHwStorage() {}
    #ifndef PC_SOFTWARE
    inline uint8_t sendByte(uint8_t data);
    void writeEnable(void);
    void waitForWriteEnd(void);
    void eraseSector(uint32_t SectorAddr);
    void eraseBulk(void);
    void writePage(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t size);
    void writeBuffer(uint8_t *pBuffer, unsigned int WriteAddr, unsigned int size);

    SmHalAbstractSpi *mSpi;

    // SmHwPowerMgrIface
    void onSleep(void);
    void onWake(uint32_t wakeSource);
    #else
    std::fstream inFile;
    #endif

    uint32_t mCount;    ///< Number of elements in the storage
};

/// @}

#endif // SM_HW_STORAGE_H
