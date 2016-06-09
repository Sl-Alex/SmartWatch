#ifndef SM_HW_SPI_SW_H_INCLUDED
#define SM_HW_SPI_SW_H_INCLUDED

#include "sm_hw_abstract_spi.h"
#include "sm_hw_abstract_gpio.h"

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
class SmHwSpiSw: public SmHwAbstractSpi
{
public:
	SmHwSpiSw()
		:mPinSck(0),
		mPinMiso(0),
		mPinMosi(0)
    {}
	virtual void init(SmHwAbstractGpio *pSck, SmHwAbstractGpio *pMiso, SmHwAbstractGpio *pMosi);
	virtual void setSs(int ss);
	virtual void resetSs(int ss);
	virtual void transfer(void * in, void * out, int size);
private:
	SmHwAbstractGpio *mPinSck;
	SmHwAbstractGpio *mPinMiso;
	SmHwAbstractGpio *mPinMosi;
};

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHwSpiSw<MODE, CFG, WIDTH>::init(SmHwAbstractGpio *pSck, SmHwAbstractGpio *pMiso, SmHwAbstractGpio *pMosi)
{
	mPinSck = pSck;
	mPinMiso = pMiso;
	mPinMosi = pMosi;
}

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHwSpiSw<MODE, CFG, WIDTH>::transfer(void * in, void * out, int size)
{
    if (WIDTH == SM_HW_SPI_WIDTH_8)
    {
        uint8_t * pOut = (uint8_t *)in;
        uint8_t * pIn  = (uint8_t *)out;
        while (size--)
        {
            uint8_t data = *pOut;
            /// @todo Implement other SPI modes
            for (int i = 0; i < 8; ++i)
            {
                if (data & 0x80)
                    mPinMosi->setPin();
                else
                    mPinMosi->resetPin();

                data <<= 1;

                if (CFG == SM_HW_SPI_CFG_FULL_DUPLEX)
                {
                    data |= mPinMiso->readPin();
                }

                mPinSck->setPin();
                mPinSck->resetPin();
            }
            if (CFG == SM_HW_SPI_CFG_FULL_DUPLEX)
            {
                *pIn = data;
                pIn++;
            }
            pOut++;
        }
    }
    if (WIDTH == SM_HW_SPI_WIDTH_16)
    {
        /// @todo Implement 16 bits
    }
}

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHwSpiSw<MODE, CFG, WIDTH>::setSs(int ss)
{
    SmHwAbstractSpi::setSs(ss);
}

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHwSpiSw<MODE, CFG, WIDTH>::resetSs(int ss)
{
    SmHwAbstractSpi::resetSs(ss);
}

#endif /* SM_HW_SPI_SW_H_INCLUDED */
