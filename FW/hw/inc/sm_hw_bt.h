#ifndef SM_HW_BT_H_INCLUDED
#define SM_HW_BT_H_INCLUDED

#include <cstdint>
#include "sm_hal_abstract_gpio.h"

class SmHwBt
{
public:
    void init(void);
    void enable(void);
    void disable(void);
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
    SmHalAbstractGpio * mPowerPin;
};

#endif /* SM_HW_BT_H_INCLUDED */
