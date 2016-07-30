#include "sm_hw_bmp180.h"

#define I2C_BMP180_ADDR     0x77


#define BMP180_CTRL_TMP          0x2E
#define BMP180_CTRL_PR0          0x34
#define BMP180_CTRL_PR1          0x74
#define BMP180_CTRL_PR2          0xB4
#define BMP180_CTRL_PR3          0xF4

#define I2C_BMP180_CHIPID   0x55

#define BMP180_TEMP_MEASUREMENT_TIME     6  ///< 6ms is a maximum temperature measurement time
#define BMP180_PRES_MEASUREMENT_TIME     30 ///< 30ms is a maximum pressure measurement time
#define BMP180_MEASUREMENT_INTERVAL 10000   ///< 10s is a measurement interval

void SmHwBmp180::init(void)
{
    mCommIface = SmHalI2c::getInstance();
    mCommIface->init();

    checkPresent();

    readCalibration();

    mMeasStep = 0;

    // Start temperature measurement
    writeReg(BMP180_CTRL, BMP180_CTRL_TMP);

    // Subscribe for the measurement results
    SmHalSysTimer::subscribe(this, BMP180_TEMP_MEASUREMENT_TIME, true);
}

bool SmHwBmp180::checkPresent(void)
{
    mPresent = (readReg(BMP180_CHIPID) == I2C_BMP180_CHIPID);

    return mPresent;
}

uint8_t SmHwBmp180::readReg(Bmp180Reg reg)
{
    uint8_t ret;
    uint8_t tmp = reg;
    mCommIface->transfer(I2C_BMP180_ADDR, &tmp, 1, &ret, 1);
    return ret;
}

bool SmHwBmp180::writeReg(Bmp180Reg reg, uint8_t data)
{
    uint8_t tmp[2];
    tmp[0] = reg; tmp[1] = data;
    return mCommIface->transfer(I2C_BMP180_ADDR, &tmp[0], 2, (uint8_t *)0, 0);
}

uint16_t SmHwBmp180::readReg16(Bmp180Reg reg)
{
    uint8_t rdata[2];
    uint16_t ret = 0;

    if (mCommIface->transfer(I2C_BMP180_ADDR, (uint8_t *)&reg, 1, rdata, 2) != false)
    {
        ret = (uint16_t)(rdata[0])<<8;
        ret |= (uint16_t)(rdata[1]);
    }

    return ret;
}

void SmHwBmp180::readCalibration(void)
{
    calData.ac1 = readReg16(BMP180_CAL_AC1);
    calData.ac2 = readReg16(BMP180_CAL_AC2);
    calData.ac3 = readReg16(BMP180_CAL_AC3);
    calData.ac4 = readReg16(BMP180_CAL_AC4);
    calData.ac5 = readReg16(BMP180_CAL_AC5);
    calData.ac6 = readReg16(BMP180_CAL_AC6);

    calData.b1 = readReg16(BMP180_CAL_B1);
    calData.b2 = readReg16(BMP180_CAL_B2);

    calData.mb = readReg16(BMP180_CAL_MB);
    calData.mc = readReg16(BMP180_CAL_MC);
    calData.md = readReg16(BMP180_CAL_MD);
}

void SmHwBmp180::calcPressure(void)
{
    int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
    uint32_t B4, B7;

    static uint32_t p_rem = 0;
    static uint32_t offset = 0;

    UT = mRawTemp;
    UP = mRawPressure;

    X1 = (UT - (int32_t)calData.ac6) * ((int32_t)calData.ac5) >> 15;
    X2 = ((int32_t)calData.mc << 11) / (X1+(int32_t)calData.md);
    B5 = X1 + X2;

    // do pressure calculations
    B6 = B5 - 4000;
    X1 = ((int32_t)calData.b2 * ( (B6 * B6)>>12 )) >> 11;
    X2 = ((int32_t)calData.ac2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = (((int32_t)calData.ac1*4 + X3) + 2) / 4;

    X1 = ((int32_t)calData.ac3 * B6) >> 13;
    X2 = ((int32_t)calData.b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = ((uint32_t)calData.ac4 * (uint32_t)(X3 + 32768)) >> 15;
    B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL );

    if (B7 < 0x80000000) {
        p = (B7 * 2) / B4;
    } else {
        p = (B7 / B4) * 2;
    }
    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;

    p = p + ((X1 + X2 + (int32_t)3791)>>4);
//    p = p *75/10000;
    p = p *75/1000;

    p_rem = p % 10;
    p = p / 10;

    if (p_rem > 7) offset = 1;
    if (p_rem < 3) offset = 0;

    p += offset;

    mPressure = p;
}

void SmHwBmp180::onTimer(uint32_t timeStamp)
{
    switch (mMeasStep)
    {
        case 0:
            // Temperature measurement is done, it's time to read result
            mRawTemp = readReg16(BMP180_CONV_RES);
            // Start pressure measurement
            writeReg(BMP180_CTRL, BMP180_CTRL_PR0);
            //Subscribe for the next measurement
            SmHalSysTimer::subscribe(this, BMP180_PRES_MEASUREMENT_TIME, false);
            mMeasStep++;
            break;
        case 1:
            // Pressure measurement interval is done, it's time to read result
            mRawPressure = readReg16(BMP180_CONV_RES);
            //Subscribe for the measurement result timeout
            SmHalSysTimer::subscribe(this, BMP180_MEASUREMENT_INTERVAL, false);
            mMeasStep++;
            break;
        case 3:
            // Start temperature measurement
            writeReg(BMP180_CTRL, BMP180_CTRL_TMP);
            //Subscribe for the measurement result timeout
            SmHalSysTimer::subscribe(this, BMP180_TEMP_MEASUREMENT_TIME, false);
            mMeasStep = 0;
            break;
    }
}
