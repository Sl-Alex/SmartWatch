#ifndef SM_HW_BMP180_H
#define SM_HW_BMP180_H

#include "sm_hal_i2c.h"
#include "sm_hal_sys_timer.h"

/// @brief BMP150 (accelerometer/magnetometer) wrapper class
class SmHwBmp180: public SmHalSysTimerIface
{
public:
    /// @brief Initialize HW and double tap interrupt output
    void init(void);

    /// @brief Simple alive check
    bool checkPresent(void);

    /// @brief Returns last alive check result
    bool isPresent(void) { return mPresent; }

    /// @brief Get last measured pressure
    uint32_t getLastPressure(void) { return mPressure; }

    SmHwBmp180(SmHwBmp180 const&) = delete;
    void operator=(SmHwBmp180 const&) = delete;
    static SmHwBmp180* getInstance()
    {
        static SmHwBmp180 instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
        return &instance;
    }
private:
    /// @todo Rewrite
    enum Bmp180Reg
    {
        //Calibration registers
        BMP180_CAL_AC1  = 0xAA, /// Calibration data start address, 11 registers 16 bits each
        BMP180_CAL_AC2  = 0xAC,
        BMP180_CAL_AC3  = 0xAE,
        BMP180_CAL_AC4  = 0xB0,
        BMP180_CAL_AC5  = 0xB2,
        BMP180_CAL_AC6  = 0xB4,
        BMP180_CAL_B1   = 0xB6,
        BMP180_CAL_B2   = 0xB8,
        BMP180_CAL_MB   = 0xBA,
        BMP180_CAL_MC   = 0xBC,
        BMP180_CAL_MD   = 0xBE, /// Last calibration register

        BMP180_CTRL     = 0xF4, /// Control register
        BMP180_CONV_RES = 0xF6, /// Conversion result registers, 2 (or 3 registers 8 bits each)
        BMP180_CHIPID   = 0xD0  ///< Chip ID, should be 0x55
    };
    /// @brief Possible initialization values of BMP180_CTRL register
    enum Bmp180CtrlReg
    {
        BMP180_CTRL_TMP = 0x2E, ///< Start temperature measurement (4.5ms)
        BMP180_CTRL_PR0 = 0x34, ///< Start pressure measurement (oversampling = 0, 4.5ms)
        BMP180_CTRL_PR1 = 0x74, ///< Start pressure measurement (oversampling = 1, 7.5ms)
        BMP180_CTRL_PR2 = 0xB4, ///< Start pressure measurement (oversampling = 2, 13.5ms)
        BMP180_CTRL_PR3 = 0xF4  ///< Start pressure measurement (oversampling = 3, 25.5ms)
    };
    SmHwBmp180() {}

    void readCalibration(void);
    void calcPressure(void);
    uint8_t readReg(Bmp180Reg reg);
    uint16_t readReg16(Bmp180Reg reg);
    bool writeReg(Bmp180Reg reg, uint8_t data);
    SmHalI2c * mCommIface;
    bool mPresent;

    struct Bmp180CalData
    {
        int16_t ac1;
        int16_t ac2;
        int16_t ac3;
        uint16_t ac4;
        uint16_t ac5;
        uint16_t ac6;
        int16_t b1;
        int16_t b2;
        int16_t mb;
        int16_t mc;
        int16_t md;
    } calData;

    uint32_t mPressure; ///< Actual pressure

    uint32_t mRawPressure;  ///< Actual raw pressure
    uint32_t mRawTemp;      ///< Actual raw temperature

    uint8_t mMeasStep;  ///< Measurement step (delay/measurement)

    // SmHalSysTimerIface event callback
    void onTimer(uint32_t timeStamp);
};

#endif // SM_HW_BMP180_H
