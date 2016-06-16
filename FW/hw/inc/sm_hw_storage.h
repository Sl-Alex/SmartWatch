#ifndef SM_HW_STORAGE_H_INCLUDED
#define SM_HW_STORAGE_H_INCLUDED

#include "sm_hal_abstract_spi.h"
#include "sm_hw_powermgr.h"

class SmHwStorage: public SmHwPowerMgrIface
{
public:
    void init(void);
    SmHwStorage(SmHwStorage const&) = delete;
    void operator=(SmHwStorage const&) = delete;
    static SmHwStorage* getInstance()
    {
        static SmHwStorage    instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return &instance;
    }
    uint32_t readId(void);
    void readData(uint8_t *pData, uint32_t flashAddr, uint32_t count);
private:
    SmHwStorage() {}
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
    void onWake(void);
};

#endif /* SM_HW_STORAGE_H_INCLUDED */
