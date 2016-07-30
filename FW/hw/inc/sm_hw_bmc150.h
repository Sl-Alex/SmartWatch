#ifndef SM_HW_BMC150_H
#define SM_HW_BMC150_H

#include "sm_hal_i2c.h"

/// @brief BMC150 (accelerometer/magnetometer) wrapper class
class SmHwBmc150
{
public:
    /// @brief Initialize HW and double tap interrupt output
    void init(void);
    
    /// @brief Simple alive check
    bool checkPresent(void);
    
    /// @brief Returns last alive check result
    bool isPresent(void) { return mPresent; }

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

#endif // SM_HW_BMC150_H
