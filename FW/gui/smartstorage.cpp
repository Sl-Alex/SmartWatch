#include "smartstorage.h"
#include <cstring>
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include <cstring>

static const int SPI_PIN_MOSI   = GPIO_Pin_15;
static const int SPI_PIN_MISO   = GPIO_Pin_14;
static const int SPI_PIN_SCK    = GPIO_Pin_13;
static const int SPI_PIN_NSS    = GPIO_Pin_12;

static const unsigned char FLASH_DUMMY_BYTE = 0xA5;


static const unsigned char FLASH_CMD_WRITE_ENABLE           =   0x06;
static const unsigned char FLASH_CMD_WRITE_DISABLE          =   0x04;
static const unsigned char FLASH_CMD_READ_IDENTIFICATION    =   0x9F;
static const unsigned char FLASH_CMD_READ_STATUS_REGISTER   =   0x05;
static const unsigned char FLASH_CMD_WRITE_STATUS_REGISTER  =   0x01;
static const unsigned char FLASH_CMD_READ_DATA_BYTES        =   0x03;
static const unsigned char FLASH_CMD_READ_DATA_BYTES_HS     =   0x0B;
static const unsigned char FLASH_CMD_PAGE_PROGRAM           =   0x02;
static const unsigned char FLASH_CMD_SECTOR_ERASE           =   0xD8;
static const unsigned char FLASH_CMD_BULK_ERASE             =   0xC7;
static const unsigned char FLASH_CMD_POWERDOWN              =   0xB9;
static const unsigned char FLASH_CMD_WAKE                   =   0xAB;

static const unsigned int  FLASH_SPI_PAGESIZE               =   0x100;

/*!< Write In Progress (WIP) flag */
static const unsigned char FLASH_FLAG_WIP   = 0x01;

/*
  Name  : CRC-8
  Poly  : 0x31    x^8 + x^5 + x^4 + 1
  Init  : 0xFF
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
*/
static const unsigned char crc8table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};

// Number of elements in a storage
unsigned char SmartStorage::nElements;
// CRC of the headers
unsigned char SmartStorage::headerCrc;
// Pointer to the array of headers
SmartStorage::StorageHeader * SmartStorage::pHeader = 0;

extern const char ext_flash[2594];

int SmartStorage::init()
{
    initHw();

//    eraseBulk();
//    writeBuffer((unsigned char *)ext_flash,0,2594);
    // Cleanup
    nElements = -1;
    if (pHeader) delete[] pHeader;

    readData(&nElements, 1, sizeof(nElements));

    if (nElements > 100) { nElements = -1; return nElements; }

    pHeader = new StorageHeader[nElements];
    readData((unsigned char *)pHeader, 2, nElements * sizeof(StorageHeader));

    if (!checkCrc())
    {
        nElements = -1;
        return nElements;
    }

    return nElements;
}

bool SmartStorage::checkCrc(void)
{
    unsigned char crc = calcCrc(&nElements,sizeof(nElements));

    for (int i = 0; i < nElements; i++)
    {
        crc = calcCrc((unsigned char *)&pHeader[i],sizeof(pHeader[0]),crc);
    }
    return (crc == headerCrc);
}

int SmartStorage::getSize(int idx)
{
    if (!isValid()) return -1;
    if (idx >= nElements) return -1;

    return pHeader[idx].size;
}

int SmartStorage::loadData(int idx, int offset, int cnt, uint8_t * pData)
{
    int i;
    if (!pData) return -1;
    if (!isValid()) return -1;
    if (idx >= nElements) return -1;
    if ((offset + cnt) > pHeader[idx].size) return -1;

    //readFlash(pHeaders[idx]->address, pHeaders[idx]->size, pData);
    /// @todo implement
    char data = 0;

    if (offset == 0){
        readData((unsigned char *)pData,pHeader[idx].address,3*sizeof(int));
        return 3*sizeof(int);
    }
    readData((unsigned char *)pData,pHeader[idx].address + 3 * sizeof(int),cnt);
    return pHeader[idx].size;
}

void SmartStorage::initHw(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /*!< sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO
    and sFLASH_SPI_SCK_GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /*!< sFLASH_SPI Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /*!< Configure sFLASH_SPI pins: MOSI/SCK */
    GPIO_InitStructure.GPIO_Pin = SPI_PIN_MOSI | SPI_PIN_SCK;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*!< Configure sFLASH_SPI pins: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI_PIN_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*!< Configure sFLASH_CS_PIN pin: sFLASH Card CS pin */
    GPIO_InitStructure.GPIO_Pin = SPI_PIN_NSS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*!< Deselect the FLASH: Chip Select high */
    GPIOB->BSRR = SPI_PIN_NSS;

    /*!< SPI configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    /*!< Enable the sFLASH_SPI  */
    SPI_Cmd(SPI2, ENABLE);

    readId();

/*
    writeEnable();

    GPIOB->BRR = SPI_PIN_NSS;
    sendByte(FLASH_CMD_WRITE_STATUS_REGISTER);
    sendByte(0x00);
    GPIOB->BSRR = SPI_PIN_NSS;

    waitForWriteEnd();*/
}


unsigned int SmartStorage::readId(void)
{
    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /*!< Select the FLASH: Chip Select low */
    GPIOB->BRR = SPI_PIN_NSS;

    /*!< Send "RDID " instruction */
    sendByte(FLASH_CMD_READ_IDENTIFICATION);

    /*!< Read a byte from the FLASH */
    Temp0 = sendByte(FLASH_DUMMY_BYTE);

    /*!< Read a byte from the FLASH */
    Temp1 = sendByte(FLASH_DUMMY_BYTE);

    /*!< Read a byte from the FLASH */
    Temp2 = sendByte(FLASH_DUMMY_BYTE);

    /*!< Deselect the FLASH: Chip Select high */
    GPIOB->BSRR = SPI_PIN_NSS;

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    return Temp;
}

unsigned char SmartStorage::sendByte(unsigned char byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

bool SmartStorage::isWriteInProgress(void)
{
    unsigned char res;
    GPIOB->BRR = SPI_PIN_NSS;

    /*!< Send "Read Status Register" instruction */
    sendByte(FLASH_CMD_READ_STATUS_REGISTER);

    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    res = ((sendByte(FLASH_DUMMY_BYTE) & FLASH_FLAG_WIP) != 0);

    /*!< Deselect the FLASH: Chip Select high */
    GPIOB->BSRR = SPI_PIN_NSS;
}

void SmartStorage::readData(unsigned char * pData, unsigned int flashAddr, unsigned int count)
{
    /*!< Select the FLASH: Chip Select low */
    GPIOB->BRR = SPI_PIN_NSS;

    /*!< Send "Read from Memory " instruction */
    sendByte(FLASH_CMD_READ_DATA_BYTES);

    /*!< Send flashAddr high nibble address byte to read from */
    sendByte((flashAddr & 0xFF0000) >> 16);
    /*!< Send flashAddr medium nibble address byte to read from */
    sendByte((flashAddr& 0xFF00) >> 8);
    /*!< Send flashAddr low nibble address byte to read from */
    sendByte(flashAddr & 0xFF);

    while (count--) /*!< while there is data to be read */
    {
        /*!< Read a byte from the FLASH */
        *pData = sendByte(FLASH_DUMMY_BYTE);
        /*!< Point to the next location where the byte read will be saved */
        pData++;
    }

    /*!< Deselect the FLASH: Chip Select high */
    GPIOB->BSRR = SPI_PIN_NSS;
}

/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void SmartStorage::eraseSector(unsigned int SectorAddr)
{
    /*!< Send write enable instruction */
    writeEnable();

    /*!< Sector Erase */
    /*!< Select the FLASH: Chip Select low */
    GPIOB->BRR = SPI_PIN_NSS;
    /*!< Send Sector Erase instruction */
    sendByte(FLASH_CMD_SECTOR_ERASE);
    /*!< Send SectorAddr high nibble address byte */
    sendByte((SectorAddr & 0xFF0000) >> 16);
    /*!< Send SectorAddr medium nibble address byte */
    sendByte((SectorAddr & 0xFF00) >> 8);
    /*!< Send SectorAddr low nibble address byte */
    sendByte(SectorAddr & 0xFF);
    /*!< Deselect the FLASH: Chip Select high */
    GPIOB->BSRR = SPI_PIN_NSS;

    /*!< Wait the end of Flash writing */
    waitForWriteEnd();
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval None
  */
void SmartStorage::eraseBulk(void)
{
  /*!< Send write enable instruction */
  writeEnable();

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  GPIOB->BRR = SPI_PIN_NSS;
  /*!< Send Bulk Erase instruction  */
  sendByte(FLASH_CMD_BULK_ERASE);
  /*!< Deselect the FLASH: Chip Select high */
  GPIOB->BSRR = SPI_PIN_NSS;

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
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void SmartStorage::writePage(unsigned char * pBuffer, unsigned int WriteAddr, unsigned int NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  writeEnable();

  /*!< Select the FLASH: Chip Select low */
  GPIOB->BRR = SPI_PIN_NSS;
  /*!< Send "Write to Memory " instruction */
  sendByte(FLASH_CMD_PAGE_PROGRAM);
  /*!< Send WriteAddr high nibble address byte to write to */
  sendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  sendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  sendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    sendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  GPIOB->BSRR = SPI_PIN_NSS;

  /*!< Wait the end of Flash writing */
  waitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
void SmartStorage::writeBuffer(unsigned char * pBuffer, unsigned int WriteAddr, unsigned int NumByteToWrite)
{
  unsigned char NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % FLASH_SPI_PAGESIZE;
  count = FLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      writePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
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
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        writePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        writePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        writePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

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

/**
  * @brief  Initiates a read data byte (READ) sequence from the Flash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the Flash still being selected. With this
  *   technique the whole content of the Flash is read with a single READ instruction.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @retval None
  */
void SmartStorage::startReadSequence(unsigned int ReadAddr)
{
  /*!< Select the FLASH: Chip Select low */
  GPIOB->BRR = SPI_PIN_NSS;

  /*!< Send "Read from Memory " instruction */
  sendByte(FLASH_CMD_READ_DATA_BYTES);

  /*!< Send the 24-bit address of the address to read from -------------------*/
  /*!< Send ReadAddr high nibble address byte */
  sendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte */
  sendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte */
  sendByte(ReadAddr & 0xFF);
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
unsigned char SmartStorage::readByte(void)
{
  return (sendByte(FLASH_DUMMY_BYTE));
}

/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void SmartStorage::writeEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  GPIOB->BRR = SPI_PIN_NSS;

  /*!< Send "Write Enable" instruction */
  sendByte(FLASH_CMD_WRITE_ENABLE);

  /*!< Deselect the FLASH: Chip Select high */
  GPIOB->BSRR = SPI_PIN_NSS;
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void SmartStorage::waitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  /*!< Select the FLASH: Chip Select low */
  GPIOB->BRR = SPI_PIN_NSS;

  /*!< Send "Read Status Register" instruction */
  sendByte(FLASH_CMD_READ_STATUS_REGISTER);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = sendByte(FLASH_DUMMY_BYTE);

  }
  while ((flashstatus & FLASH_FLAG_WIP) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  GPIOB->BSRR = SPI_PIN_NSS;
}

unsigned char SmartStorage::calcCrc(unsigned char * pData, unsigned char count, unsigned char crc)
{
    while (count--)
        crc = crc8table[crc ^ *pData++];

    return crc;
}
