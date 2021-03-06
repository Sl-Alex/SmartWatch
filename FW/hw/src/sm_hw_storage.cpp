#include "sm_hw_storage.h"
#include "sm_crc.h"

#ifndef PC_SOFTWARE
#include "sm_hal_gpio.h"
#include "sm_hal_spi_hw.h"

#define MOSI_PORT_BASE  GPIOB_BASE
#define MOSI_PIN        15
#define MISO_PORT_BASE  GPIOB_BASE
#define MISO_PIN        14
#define SCK_PORT_BASE   GPIOB_BASE
#define SCK_PIN         13
#define SS_PORT_BASE    GPIOB_BASE
#define SS_PIN          12

#define SPI_BASE        SPI2_BASE

// Flash layout
#define FW_DATA_SECTOR_START    0
#define FW_DATA_SECTOR_END      6
#define USER_SETTINGS_SECTOR    7

#define FLASH_DUMMY_BYTE                0xA5

#define FLASH_CMD_WRITE_ENABLE          0x06
#define FLASH_CMD_WRITE_DISABLE         0x04
#define FLASH_CMD_READ_IDENTIFICATION   0x9F
#define FLASH_CMD_READ_STATUS_REGISTER  0x05
#define FLASH_CMD_WRITE_STATUS_REGISTER 0x01
#define FLASH_CMD_READ_DATA_BYTES       0x03
#define FLASH_CMD_READ_DATA_BYTES_HS    0x0B
#define FLASH_CMD_PAGE_PROGRAM          0x02
#define FLASH_CMD_SECTOR_ERASE          0xD8
#define FLASH_CMD_BULK_ERASE            0xC7
#define FLASH_CMD_POWERDOWN             0xB9
#define FLASH_CMD_WAKE                  0xAB

#define FLASH_SPI_PAGESIZE              0x100

/*!< Write In Progress (WIP) flag */
#define FLASH_FLAG_WIP                  0x01

//#include "update.inc"

void SmHwStorage::init(void)
{
    mSpi = new SmHalSpiHw<SPI_BASE, SM_HAL_SPI_CFG_FULL_DUPLEX>();

    SmHalGpio<MOSI_PORT_BASE, MOSI_PIN> spiMosi;
    SmHalGpio<MISO_PORT_BASE, MISO_PIN> spiMiso;
    SmHalGpio<SCK_PORT_BASE,  SCK_PIN>  spiSck;

    spiMosi.setModeSpeed(SM_HAL_GPIO_MODE_AF_PP,SM_HAL_GPIO_SPEED_50M);
    spiMiso.setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT,SM_HAL_GPIO_SPEED_50M);
    spiSck.setModeSpeed(SM_HAL_GPIO_MODE_AF_PP,SM_HAL_GPIO_SPEED_50M);

    mSpi->setSsPin(new SmHalGpio<SS_PORT_BASE, SS_PIN>());
    ((SmHalSpiHw<SPI_BASE, SM_HAL_SPI_CFG_FULL_DUPLEX> *)mSpi)->init(SM_HAL_SPI_MODE3, SM_HAL_SPI_WIDTH_8);

    SmHwPowerMgr::getInstance()->subscribe(this);
    onWake(0);

    readId();

//    eraseBulk();
//    writeBuffer(update_bin, 0, update_bin_len);

    // Read total number of elements in the storage
    readData(0, (uint8_t *)&mCount, sizeof(mCount));

    // Verify CRC
    uint32_t crc32 = SmCrc::calc32(0xFFFFFFFF, (uint8_t *)&mCount, sizeof(mCount));
    uint32_t addr = sizeof(mCount);
    SmHwStorageElementInfo elementInfo;
    // Next element offset. We can check if it is correct
    uint32_t nextOffset = sizeof(mCount) + sizeof(elementInfo)*mCount + sizeof(crc32);
    for (uint32_t i = 0; i < mCount; ++i)
    {
        // Read element info
        readData(addr, (uint8_t *)&elementInfo, sizeof(elementInfo));
        crc32 = SmCrc::calc32(crc32, (uint8_t *)&elementInfo, sizeof(elementInfo));
        addr += sizeof(elementInfo);

        if(elementInfo.offset != nextOffset)
        {
            mCount = 0;
            return;
        }

        // Calculate next element offset
        nextOffset += elementInfo.size;
    }
    uint32_t crcStored;
    readData(addr, (uint8_t *)&crcStored, sizeof(crcStored));
    if (crcStored != crc32)
        mCount = 0;
}

uint32_t SmHwStorage::getElementSize(uint8_t element)
{
    SmHwStorageElementInfo elementInfo;

    if (getElementInfo(element, &elementInfo) == false)
        return 0;

    return elementInfo.size;
}

bool SmHwStorage::getElementInfo(uint8_t element, SmHwStorageElementInfo * info)
{
    if (element >= mCount)
        return false;

    // Read element info
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)info, sizeof(SmHwStorageElementInfo));

    return true;
}

void SmHwStorage::readElement(uint8_t element, uint32_t offset, uint8_t * pData, uint32_t size)
{
    if (element >= mCount)
        return;

    uint32_t startOffset;

    // Read element info
    SmHwStorageElementInfo elementInfo;
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)&elementInfo, sizeof(elementInfo));

    // Read data
    readData(elementInfo.offset + offset, pData, size);
}


inline uint8_t SmHwStorage::sendByte(uint8_t data)
{
    uint8_t ret;
    mSpi->transfer(&ret, &data, 1);
    return ret;
}

uint32_t SmHwStorage::readId(void)
{
    uint32_t result = 0;

    // Select the FLASH: Chip Select low
    mSpi->resetSs();

    // Send command
    sendByte(FLASH_CMD_READ_IDENTIFICATION);

    // Read 3 bytes
    result |= sendByte(FLASH_DUMMY_BYTE);
    result <<= 8;
    result |= sendByte(FLASH_DUMMY_BYTE);
    result <<= 8;
    result |= sendByte(FLASH_DUMMY_BYTE);

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();

    return result;
}

void SmHwStorage::readData(uint32_t offset, uint8_t *pData, uint32_t size)
{
    uint8_t res = 0;
    uint8_t data;

    // Select the FLASH: Chip Select low
    mSpi->resetSs();

    // Send "Read from Memory " instruction
    sendByte(FLASH_CMD_READ_DATA_BYTES);

    // Send offset high nibble address byte to read from
    sendByte((offset & 0xFF0000) >> 16);
    // Send offset medium nibble address byte to read from
    sendByte((offset & 0xFF00) >> 8);
    // Send offset low nibble address byte to read from
    sendByte(offset & 0xFF);

    data = FLASH_DUMMY_BYTE;
    while (size--) // while there is data to be read
    {
        // Read a byte from the FLASH
        mSpi->transfer(pData, &data, 1);

        // Point to the next location where the byte read will be saved
        pData++;
    }

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();
}

void readSettings(SmHwStorageSettings * settings)
{

}

void writeSettings(const SmHwStorageSettings * const settings)
{

}

/// @brief Enable write.
void SmHwStorage::writeEnable(void)
{
    // Select the FLASH: Chip Select low
    mSpi->resetSs();

    // Send "Write Enable" instruction
    sendByte(FLASH_CMD_WRITE_ENABLE);

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();
}


/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void SmHwStorage::eraseSector(uint32_t SectorAddr)
{
    // Send write enable instruction
    writeEnable();

    // Sector Erase
    // Select the FLASH: Chip Select low
    mSpi->resetSs();
    // Send Sector Erase instruction
    sendByte(FLASH_CMD_SECTOR_ERASE);
    // Send SectorAddr high nibble address byte
    sendByte((SectorAddr & 0xFF0000) >> 16);
    // Send SectorAddr medium nibble address byte
    sendByte((SectorAddr & 0xFF00) >> 8);
    // Send SectorAddr low nibble address byte
    sendByte(SectorAddr & 0xFF);
    // Deselect the FLASH: Chip Select high
    mSpi->setSs();

    // Wait the end of Flash writing
    waitForWriteEnd();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void SmHwStorage::waitForWriteEnd(void)
{
    uint8_t flashstatus = 0;

    /*!< Select the FLASH: Chip Select low */
    mSpi->resetSs();

    /*!< Send "Read Status Register" instruction */
    sendByte(FLASH_CMD_READ_STATUS_REGISTER);

    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
        // Send a dummy byte to generate the clock needed by the FLASH
        // and put the value of the status register in FLASH_Status variable
        flashstatus = sendByte(FLASH_DUMMY_BYTE);

    }
    while ((flashstatus & FLASH_FLAG_WIP) == SET); /* Write in progress */

    // Deselect the FLASH: Chip Select high */
    mSpi->setSs();
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval None
  */
void SmHwStorage::eraseBulk(void)
{
    /*!< Send write enable instruction */
    writeEnable();

    /*!< Bulk Erase */
    /*!< Select the FLASH: Chip Select low */
    mSpi->resetSs();
    /*!< Send Bulk Erase instruction  */
    sendByte(FLASH_CMD_BULK_ERASE);
    /*!< Deselect the FLASH: Chip Select high */
    mSpi->setSs();

    /*!< Wait the end of Flash writing */
    waitForWriteEnd();
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  size: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void SmHwStorage::writePage(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t size)
{
    // Enable the write access to the FLASH
    writeEnable();

    mSpi->resetSs();

    // Send "Write to Memory " instruction
    sendByte(FLASH_CMD_PAGE_PROGRAM);

    // Send WriteAddr
    sendByte((WriteAddr & 0xFF0000) >> 16);
    sendByte((WriteAddr & 0xFF00) >> 8);
    sendByte(WriteAddr & 0xFF);

    while (size--)
    {
        sendByte(*pBuffer);
        pBuffer++;
    }

    // Deselect the FLASH: Chip Select high
    mSpi->setSs();

    waitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  size: number of bytes to write to the FLASH.
  * @retval None
  */
void SmHwStorage::writeBuffer(uint8_t *pBuffer, unsigned int WriteAddr, unsigned int size)
{
    unsigned char NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % FLASH_SPI_PAGESIZE;
    count = FLASH_SPI_PAGESIZE - Addr;
    NumOfPage =  size / FLASH_SPI_PAGESIZE;
    NumOfSingle = size % FLASH_SPI_PAGESIZE;

    if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
    {
    if (NumOfPage == 0) /*!< size < sFLASH_PAGESIZE */
    {
        writePage(pBuffer, WriteAddr, size);
    }
    else /*!< size > sFLASH_PAGESIZE */
    {
        while (NumOfPage--)
        {
            writePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
            WriteAddr +=  FLASH_SPI_PAGESIZE;
            pBuffer += FLASH_SPI_PAGESIZE;
        }

        writePage(pBuffer, WriteAddr, NumOfSingle);
    }
    }
    else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
    {
        if (NumOfPage == 0) /*!< size < sFLASH_PAGESIZE */
        {
            if (NumOfSingle > count) /*!< (size + WriteAddr) > sFLASH_PAGESIZE */
            {
                temp = NumOfSingle - count;

                writePage(pBuffer, WriteAddr, count);
                WriteAddr +=  count;
                pBuffer += count;

                writePage(pBuffer, WriteAddr, temp);
            }
            else
            {
                writePage(pBuffer, WriteAddr, size);
            }
        }
        else /*!< size > sFLASH_PAGESIZE */
        {
            size -= count;
            NumOfPage =  size / FLASH_SPI_PAGESIZE;
            NumOfSingle = size % FLASH_SPI_PAGESIZE;

            writePage(pBuffer, WriteAddr, count);
            WriteAddr +=  count;
            pBuffer += count;

            while (NumOfPage--)
            {
                writePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
                WriteAddr +=  FLASH_SPI_PAGESIZE;
                pBuffer += FLASH_SPI_PAGESIZE;
            }

            if (NumOfSingle != 0)
            {
                writePage(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

void SmHwStorage::onSleep(void)
{
    mSpi->resetSs();
    sendByte(FLASH_CMD_POWERDOWN);
    mSpi->setSs();
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void SmHwStorage::onWake(uint32_t wakeSource)
{
    mSpi->resetSs();
    sendByte(FLASH_CMD_WAKE);
    /// @todo It would be fine to make a calibrated 3us delay.
    for (uint32_t i = 0; i < 1000; i++)
        mSpi->setSs();
}
#pragma GCC pop_options
#else
void SmHwStorage::init(void)
{
    inFile.open("../../Resources/Flash/update.bin", std::ios::in | std::ios::binary);

    // Read total number of elements in the storage
    readData(0, (uint8_t *)&mCount, sizeof(mCount));

    // Verify CRC
    uint32_t crc32 = SmCrc::calc32(0xFFFFFFFF, (uint8_t *)&mCount, sizeof(mCount));
    uint32_t addr = sizeof(mCount);
    SmHwStorageElementInfo elementInfo;
    // Next element offset. We can check if it is correct
    uint32_t nextOffset = sizeof(mCount) + sizeof(elementInfo)*mCount + sizeof(crc32);
    for (uint32_t i = 0; i < mCount; ++i)
    {
        // Read element info
        readData(addr, (uint8_t *)&elementInfo, sizeof(elementInfo));
        crc32 = SmCrc::calc32(crc32, (uint8_t *)&elementInfo, sizeof(elementInfo));
        addr += sizeof(elementInfo);

        if(elementInfo.offset != nextOffset)
        {
            mCount = 0;
            return;
        }

        // Calculate next element offset
        nextOffset += elementInfo.size;
    }
    uint32_t crcStored;
    readData(addr, (uint8_t *)&crcStored, sizeof(crcStored));
    if (crcStored != crc32)
        mCount = 0;
}

uint32_t SmHwStorage::getElementSize(uint8_t element)
{
    SmHwStorageElementInfo elementInfo;

    if (getElementInfo(element, &elementInfo) == false)
        return 0;

    return elementInfo.size;
}

bool SmHwStorage::getElementInfo(uint8_t element, SmHwStorageElementInfo * info)
{
    if (element >= mCount)
        return false;

    // Read element info
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)info, sizeof(SmHwStorageElementInfo));

    return true;
}

void SmHwStorage::readElement(uint8_t element, uint32_t offset, uint8_t * pData, uint32_t size)
{
    if (element >= mCount)
        return;

    // Read element info
    SmHwStorageElementInfo elementInfo;
    readData(sizeof(mCount) + sizeof(SmHwStorageElementInfo)*element, (uint8_t *)&elementInfo, sizeof(elementInfo));

    // Read data
    readData(elementInfo.offset + offset, pData, size);
}

void SmHwStorage::readData(uint32_t offset, uint8_t *pData, uint32_t size)
{
    // Reset text file to the beginning
    inFile.seekg(offset, std::ios::beg);

    // Read data
    inFile.read((char *)pData,size);
}
#endif
