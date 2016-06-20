#include "sm_hw_bmc150.h"


#define I2C_ADDR_ACC    0x10
#define I2C_ADDR_MAGN   0x12

#define I2C_ACC_CHIPID  0xFA

void SmHwBmc150::init(void)
{
    mCommIface = SmHalI2c::getInstance();
    mCommIface->init();

    writeReg(BMC150_ACC_INT_EN_0, 0x10); // 0x20 - single, 0x10 - double tap
    writeReg(BMC150_ACC_INT_MAP_0, 0x10);
    writeReg(BMC150_ACC_INT_OUT_CTRL, 0x00); // Active low for all int outputs



    mPresent = false;
}

bool SmHwBmc150::checkPresent(void)
{
    mPresent = (readReg(BMC150_ACC_CHIPID) == I2C_ACC_CHIPID);

    return mPresent;
}

uint8_t SmHwBmc150::readReg(Bmc150AccReg reg)
{
    uint8_t ret;
    uint8_t tmp = reg;
    mCommIface->transfer(I2C_ADDR_ACC, &tmp, 1, &ret, 1);
    return ret;
}

bool SmHwBmc150::writeReg(Bmc150AccReg reg, uint8_t data)
{
    uint8_t tmp[2];
    tmp[0] = reg; tmp[1] = data;
    return mCommIface->transfer(I2C_ADDR_ACC, &tmp[0], 2, (uint8_t *)0, 0);
}
