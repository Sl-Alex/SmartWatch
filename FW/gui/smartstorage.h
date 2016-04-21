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

    static int nElements;              // Number of elements in a storage, -1 means failure
    static StorageHeader * pHeader;	// Pointer to the array of headers
    static unsigned int headerCrc;				// CRC of all above

    static void initHw(void);
    static unsigned char sendByte(unsigned char byte);
    static bool isWriteInProgress(void);
    static void readData(unsigned char * pData, unsigned int flashAddr, unsigned int count);

};

#endif // SMARTSTORAGE_H
