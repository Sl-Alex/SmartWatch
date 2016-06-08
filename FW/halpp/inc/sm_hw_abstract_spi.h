#ifndef SM_HW_ABSTRACT_SPI_H_INCLUDED
#define SM_HW_ABSTRACT_SPI_H_INCLUDED

#include <stdint.h>

enum SpiConfig {
    SM_HW_SPI_CFG_FULL_DUPLEX,
    SM_HW_SPI_CFG_OUT
};
enum SpiWidth {
    SM_HW_SPI_WIDTH_8,
    SM_HW_SPI_WIDTH_16
};
enum SpiMode {
    SM_HW_SPI_MODE0 = 0,    ///< CPOL = 0, CPHA = 0
    SM_HW_SPI_MODE1,        ///< CPOL = 0, CPHA = 1
    SM_HW_SPI_MODE2,        ///< CPOL = 1, CPHA = 0
    SM_HW_SPI_MODE3         ///< CPOL = 1, CPHA = 1
};

class SmHwAbstractSpi
{
public:
    SmHwAbstractSpi()
        :mSsPins(0),
        mSsCount(0)
    {}
	virtual void setMode(SpiMode spiMode){};
	virtual void setWidth(SpiWidth width){};
	virtual void transfer(void * in, void * out, int size) = 0;
	virtual ~SmHwAbstractSpi() {};
	virtual void setSsPins(SmHwGpioPin *pSs , int ssCount)
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
    SmHwGpioPin *mSsPins;
    int mSsCount;
};

#endif /* SM_HW_ABSTRACT_SPI_H_INCLUDED */
