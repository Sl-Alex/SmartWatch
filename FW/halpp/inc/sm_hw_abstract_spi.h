#ifndef SM_HW_ABSTRACT_SPI_H_INCLUDED
#define SM_HW_ABSTRACT_SPI_H_INCLUDED

#include <stdint.h>
#include "sm_hw_abstract_gpio.h"

enum SpiConfig {
    SM_HW_SPI_CFG_FULL_DUPLEX = 0x0000,
    SM_HW_SPI_CFG_OUT         = 0xC000
};
enum SpiWidth {
    SM_HW_SPI_WIDTH_8  = 0x0000,
    SM_HW_SPI_WIDTH_16 = 0x0800
};
enum SpiMode {
    SM_HW_SPI_MODE0 = 0,  ///< CPOL = 0, CPHA = 0 (default low, first edge)
    SM_HW_SPI_MODE1 = 1,  ///< CPOL = 0, CPHA = 1 (default low, second edge)
    SM_HW_SPI_MODE2 = 2,  ///< CPOL = 1, CPHA = 0 (default hight, first edge)
    SM_HW_SPI_MODE3 = 3   ///< CPOL = 1, CPHA = 1 (default hight, second edge)
};
enum SpiMsMode {
	SPI_HW_SPI_MSMODE_MASTER = 0x0104,
	SPI_HW_SPI_MSMODE_SLAVE = 0x0000
};
enum SpiNssMode {
	SPI_HW_SPI_NSS_MODE_SOFT = 0x0200,
	SPI_HW_SPI_NSS_MODE_HARD = 0x0000
};

class SmHwAbstractSpi
{
public:
    SmHwAbstractSpi()
        :mSsPins(0),
        mSsCount(0)
    {}
	virtual void transfer(void * in, void * out, int size) = 0;
	virtual ~SmHwAbstractSpi() {};
	virtual void setSsPins(SmHwAbstractGpio *pSs , int ssCount)
	{
	    mSsPins = pSs;
	    mSsCount = ssCount;
	    for (int i = 0; i < ssCount; ++i)
        {
            pSs->setModeSpeed(SM_HW_GPIO_MODE_OUT_PP, SM_HW_GPIO_SPEED_50M);
        }
	}
	virtual void setSs(void)
	{
	    mSsPins[0].setPin();
	}
	virtual void setSs(int ss)
	{
	    mSsPins[ss].setPin();
	}
	virtual void resetSs(void)
	{
	    mSsPins[0].resetPin();
	}
	virtual void resetSs(int ss)
	{
	    mSsPins[ss].resetPin();
	}
private:
    SmHwAbstractGpio *mSsPins;
    int mSsCount;
};

#endif /* SM_HW_ABSTRACT_SPI_H_INCLUDED */
