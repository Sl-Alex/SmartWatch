#ifndef SM_HW_STORAGE_H_INCLUDED
#define SM_HW_STORAGE_H_INCLUDED

#include "sm_hal_abstract_spi.h"

class SmHwStorage
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
private:
    SmHwStorage() {}
    SmHalAbstractSpi *mSpi;
};

#endif /* SM_HW_STORAGE_H_INCLUDED */
