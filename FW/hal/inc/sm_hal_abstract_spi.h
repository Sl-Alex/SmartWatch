#ifndef SM_HAL_ABSTRACT_SPI_H_INCLUDED
#define SM_HAL_ABSTRACT_SPI_H_INCLUDED

#include <cstdint>
#include "sm_hal_abstract_gpio.h"

enum SpiConfig {
    SM_HAL_SPI_CFG_FULL_DUPLEX = 0x0000,
    SM_HAL_SPI_CFG_OUT         = 0xC000
};
enum SpiWidth {
    SM_HAL_SPI_WIDTH_8  = 0x0000,
    SM_HAL_SPI_WIDTH_16 = 0x0800
};
enum SpiMode {
    SM_HAL_SPI_MODE0 = 0,  ///< CPOL = 0, CPHA = 0 (default low, first edge)
    SM_HAL_SPI_MODE1 = 1,  ///< CPOL = 0, CPHA = 1 (default low, second edge)
    SM_HAL_SPI_MODE2 = 2,  ///< CPOL = 1, CPHA = 0 (default hight, first edge)
    SM_HAL_SPI_MODE3 = 3   ///< CPOL = 1, CPHA = 1 (default hight, second edge)
};
enum SpiMsMode {
    SPI_HAL_SPI_MSMODE_MASTER = 0x0104,
    SPI_HAL_SPI_MSMODE_SLAVE = 0x0000
};
enum SpiNssMode {
    SPI_HAL_SPI_NSS_MODE_SOFT = 0x0200,
    SPI_HAL_SPI_NSS_MODE_HARD = 0x0000
};

class SmHalAbstractSpi
{
public:
    SmHalAbstractSpi()
        :mSsPin(0)
    {}
    virtual void transfer(void * in, void * out, int size) = 0;
    virtual void init(void)
    {
        mSsPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_50M);
        mSsPin->setPin();
    }
    virtual void deInit(void)
    {
        mSsPin->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT, SM_HAL_GPIO_SPEED_50M);
        mSsPin->resetPin();
    };
    virtual ~SmHalAbstractSpi() {};
    virtual void setSsPin(SmHalAbstractGpio *pSs)
    {
        mSsPin = pSs;
        init();
    }
    virtual void setSs(void)
    {
        mSsPin->setPin();
    }
    virtual void resetSs(void)
    {
        mSsPin->resetPin();
    }
private:
    SmHalAbstractGpio *mSsPin;
};

#endif /* SM_HAL_ABSTRACT_SPI_H_INCLUDED */
