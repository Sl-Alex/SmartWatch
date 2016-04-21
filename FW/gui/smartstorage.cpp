#include "smartstorage.h"
#include <cstring>
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"

extern const char image_data_font[1536];
extern const char menu[13][128];

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

/*!< Write In Progress (WIP) flag */
static const unsigned char FLASH_FLAG_WIP   = 0x01;

// Number of elements in a storage, -1 means failure
int SmartStorage::nElements;
// Pointer to the array of headers
SmartStorage::StorageHeader * SmartStorage::pHeader = 0;
// CRC of all above
unsigned int SmartStorage::headerCrc;

int SmartStorage::init()
{
    initHw();
    // Cleanup
    nElements = -1;
    if (pHeader) delete[] pHeader;

    //readFlash(address = 0,size = sizeof(nElements),&nElements);
    /// @todo temp
    nElements = 16;

    if (nElements > 100) { nElements = -1; return nElements; }

    pHeader = new StorageHeader[nElements];

    //readFlash(address = 1,size = nElements * sizeof(StorageHeader), pHeader);
    /// @todo temp
    #if (BPP == 8)
    pHeader[0].address = 100;
    pHeader[0].size = 19*19 + 12;//*sizeof(int);
    pHeader[1].address = 200;
    pHeader[1].size = 5*5 + 12;
    pHeader[2].address = 300;
    pHeader[2].size = 6*8*255*sizeof(char) + 12;
    #elif (BPP == 1)
    pHeader[0].address = 100;
    pHeader[0].size = 19*3 + 12;//*sizeof(int);
    pHeader[1].address = 200;
    pHeader[1].size = 5*1 + 12;
    pHeader[2].address = 300;
    pHeader[2].size = 6*1*256*sizeof(char) + 12;

    pHeader[3].address = 400;
    pHeader[3].size = 32*4*sizeof(char) + 12;
    pHeader[4].address = 400;
    pHeader[4].size = 32*4*sizeof(char) + 12;
    pHeader[5].address = 400;
    pHeader[5].size = 32*4*sizeof(char) + 12;
    pHeader[6].address = 400;
    pHeader[6].size = 32*4*sizeof(char) + 12;
    pHeader[7].address = 400;
    pHeader[7].size = 32*4*sizeof(char) + 12;
    pHeader[8].address = 400;
    pHeader[8].size = 32*4*sizeof(char) + 12;
    pHeader[9].address = 400;
    pHeader[9].size = 32*4*sizeof(char) + 12;
    pHeader[10].address = 400;
    pHeader[10].size = 32*4*sizeof(char) + 12;
    pHeader[11].address = 400;
    pHeader[11].size = 32*4*sizeof(char) + 12;
    pHeader[12].address = 400;
    pHeader[12].size = 32*4*sizeof(char) + 12;
    pHeader[13].address = 400;
    pHeader[13].size = 32*4*sizeof(char) + 12;
    pHeader[14].address = 400;
    pHeader[14].size = 32*4*sizeof(char) + 12;
    pHeader[15].address = 400;
    pHeader[15].size = 32*4*sizeof(char) + 12;
    #endif
    if (!checkCrc())
    {
        nElements = -1;
        return nElements;
    }

    return nElements;
}

bool SmartStorage::checkCrc(void)
{
    return true;
}

int SmartStorage::getSize(int idx)
{
    if (!isValid()) return -1;
    if (idx >= nElements) return -1;

    return pHeader[idx].size;
}

int SmartStorage::loadData(int idx, int offset, int cnt, char * pData)
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
        if (idx == 0)
        {
            ((int *)pData)[0] = 19;
            ((int *)pData)[1] = 19;
            #if (BPP == 8)
            ((int *)pData)[2] = 19*19;
            #else
            ((int *)pData)[2] = 19*3;
            #endif
        }
        else if (idx == 1)
        {
            ((int *)pData)[0] = 5;
            ((int *)pData)[1] = 5;
            #if (BPP == 8)
            ((int *)pData)[2] = 5*5;
            #else
            ((int *)pData)[2] = 5*1;
            #endif
        }
        else if (idx == 2)
        {
            ((int *)pData)[0] = 6UL*256UL;
            ((int *)pData)[1] = 8UL;
            #if (BPP == 8)
            ((int *)pData)[2] = 6*256*8UL;
            #else
            ((int *)pData)[2] = 6*256*1UL;
            #endif
        }
        else if ((idx >= 3) && (idx <= 16))
        {
            ((int *)pData)[0] = 32;
            ((int *)pData)[1] = 32;
            #if (BPP == 8)
            ((int *)pData)[2] = 32*32;
            #else
            ((int *)pData)[2] = 32*4;
            #endif
        }
    }
    if (offset == 0) return 3*sizeof(int);
    if (idx == 0)
        for (int i = 0; i < cnt; i++)
        {
            pData[i] = data;
            #if (BPP == 8)
            data+=64;
            #else
            data = 1 - data;
            #endif
        }
    else if (idx == 1)
        for (int i = 0; i < cnt; i++)
        {
            pData[i] = 255;
            #if (BPP == 8)
            data = 255 - data;
            #endif
        }
    else if (idx == 2)
    {
        #if (BPP == 8)
        pData[0] = 255;
        pData[1] = 255;
        pData[2] = 255;
        pData[3] = 255;
        pData[4] = 255;
        pData[5] = 255;
        pData[255*6*7+0] = 255;
        pData[255*6*7+1] = 128;
        pData[255*6*7+2] = 128;
        pData[255*6*7+3] = 128;
        pData[255*6*7+4] = 128;
        pData[255*6*7+5] = 255;

        pData[6+0] = 255;
        pData[6+1] = 128;
        pData[6+2] = 255;
        pData[6+3] = 255;
        pData[6+4] = 128;
        pData[6+5] = 255;
        pData[255*6*7+6+0] = 255;
        pData[255*6*7+6+1] = 255;
        pData[255*6*7+6+2] = 128;
        pData[255*6*7+6+3] = 128;
        pData[255*6*7+6+4] = 255;
        pData[255*6*7+6+5] = 255;
        #else
        for (i =0;i< 256*6; i++)
        {

            *pData = image_data_font[i];
            pData++;
        }
        /*pData[0]=0x80;
        pData[1]=0x70;
        pData[2]=0x0F;
        pData[3]=0x01;
        pData[4]=0xFF;
        pData[5]=0x81;

        pData[6+0]=0x7E;
        pData[6+1]=0x81;
        pData[6+2]=0xBD;
        pData[6+3]=0xBD;
        pData[6+4]=0x81;
        pData[6+5]=0x7E;*/
        #endif
    }
    else if ((idx >= 3) && (idx <= 16))
    {
        #if (BPP == 1)
        for (i =0;i< 32*4; i++)
        {

            *pData = menu[idx - 3][i];
            pData++;
        }
        #endif
    }
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
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    /*!< Enable the sFLASH_SPI  */
    SPI_Cmd(SPI2, ENABLE);

    readId();
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
