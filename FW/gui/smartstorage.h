#ifndef SMARTSTORAGE_H
#define SMARTSTORAGE_H

class SmartStorage
{
public:
    struct StorageHeader
    {
        int size;
        int address;
    };

    SmartStorage()
    {}

    static int init(void);	// Initialize members, returns number of elements
    static int getSize(int idx); // Get the size of the object idx, -1 if fails
    static int loadData(int idx, int offset, int cnt, char * pData);	// Read the data from the flash to the pData. -1 if fails
    static inline bool isValid(void) {return nElements >=0; }

    static unsigned int readId(void);

private:
    static bool checkCrc(void); // Check CRC of the members

    static unsigned char  nElements;    // Number of elements in a storage
    static unsigned char headerCrc;		// CRC of all above
    static StorageHeader * pHeader;	// Pointer to the array of headers

    static void initHw(void);
    static unsigned char sendByte(unsigned char byte);
    static bool isWriteInProgress(void);
    static void readData(unsigned char * pData, unsigned int flashAddr, unsigned int count);
    static void eraseSector(unsigned int SectorAddr);
    static void eraseBulk(void);
    static void writePage(unsigned char * pBuffer, unsigned int WriteAddr, unsigned int NumByteToWrite);
    static void writeBuffer(unsigned char * pBuffer, unsigned int WriteAddr, unsigned int NumByteToWrite);
    static void writeEnable(void);
    static void waitForWriteEnd(void);
    static void startReadSequence(unsigned int ReadAddr);
    static unsigned char readByte(void);
    static unsigned char calcCrc(unsigned char * pData, unsigned char count, unsigned char crc = 0xFF);
};

#endif // SMARTSTORAGE_H
