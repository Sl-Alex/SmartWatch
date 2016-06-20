#ifndef SM_HW_BMC150_H_INCLUDED
#define SM_HW_BMC150_H_INCLUDED

#include "sm_hal_i2c.h"

class SmHwBmc150
{
public:
    void init(void);
    bool isPresent(void) { return mPresent; }
    bool checkPresent(void);

    SmHwBmc150(SmHwBmc150 const&) = delete;
    void operator=(SmHwBmc150 const&) = delete;
    static SmHwBmc150* getInstance()
    {
        static SmHwBmc150 instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
        return &instance;
    }
private:
    enum Bmc150AccReg
    {
        BMC150_ACC_CHIPID        = 0x00,
        BMC150_ACC_INT_EN_0      = 0x16,
        BMC150_ACC_INT_EN_1      = 0x17,
        BMC150_ACC_INT_EN_2      = 0x18,
        BMC150_ACC_INT_MAP_0     = 0x19,
        BMC150_ACC_INT_MAP_1     = 0x1A,
        BMC150_ACC_INT_MAP_2     = 0x1B,
        BMC150_ACC_INT_OUT_CTRL  = 0x20,
        BMC150_ACC_INT_RST_LATCH = 0x21
    };
    SmHwBmc150() {}
    uint8_t readReg(Bmc150AccReg reg);
    bool writeReg(Bmc150AccReg reg, uint8_t data);
    SmHalI2c * mCommIface;
    bool mPresent;
};

#endif /* SM_HW_BMC150_H_INCLUDED */
