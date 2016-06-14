#ifndef SM_HAL_SPI_SW_H_INCLUDED
#define SM_HAL_SPI_SW_H_INCLUDED

#include "sm_hal_abstract_spi.h"
#include "sm_hal_abstract_gpio.h"

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
class SmHalSpiSw: public SmHalAbstractSpi
{
public:
    SmHalSpiSw()
        :mPinSck(0),
        mPinMiso(0),
        mPinMosi(0)
    {}
    void init(void);
    void deInit(void);
    virtual void init(SmHalAbstractGpio *pSck, SmHalAbstractGpio *pMiso, SmHalAbstractGpio *pMosi);
    virtual void transfer(void * in, void * out, int size);
private:
    SmHalAbstractGpio *mPinSck;
    SmHalAbstractGpio *mPinMiso;
    SmHalAbstractGpio *mPinMosi;
};

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHalSpiSw<MODE, CFG, WIDTH>::deInit(void)
{
    SmHalAbstractSpi::deInit();
    if (mPinMiso)
        mPinMiso->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT,SM_HAL_GPIO_SPEED_2M);
    if (mPinMosi)
        mPinMosi->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT,SM_HAL_GPIO_SPEED_2M);
    if (mPinSck)
        mPinSck->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT,SM_HAL_GPIO_SPEED_2M);
}

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHalSpiSw<MODE, CFG, WIDTH>::init(void)
{
    SmHalAbstractSpi::init();
    if (mPinMiso)
        mPinMiso->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT,SM_HAL_GPIO_SPEED_50M);
    if (mPinMosi)
        mPinMosi->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP,SM_HAL_GPIO_SPEED_50M);
    if (mPinSck)
        mPinSck->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP,SM_HAL_GPIO_SPEED_50M);
}

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHalSpiSw<MODE, CFG, WIDTH>::init(SmHalAbstractGpio *pSck, SmHalAbstractGpio *pMiso, SmHalAbstractGpio *pMosi)
{
    mPinSck = pSck;
    mPinMiso = pMiso;
    mPinMosi = pMosi;
    init();
}

template<SpiMode MODE, SpiConfig CFG, SpiWidth WIDTH>
void SmHalSpiSw<MODE, CFG, WIDTH>::transfer(void * in, void * out, int size)
{
    if (WIDTH == SM_HAL_SPI_WIDTH_8)
    {
        uint8_t * pOut = (uint8_t *)out;
        uint8_t * pIn  = (uint8_t *)in;
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

                if (CFG == SM_HAL_SPI_CFG_FULL_DUPLEX)
                {
                    data |= mPinMiso->readPin();
                }

                mPinSck->setPin();
                mPinSck->resetPin();
            }
            if (CFG == SM_HAL_SPI_CFG_FULL_DUPLEX)
            {
                *pIn = data;
                pIn++;
            }
            pOut++;
        }
    }
    if (WIDTH == SM_HAL_SPI_WIDTH_16)
    {
        /// @todo Implement 16 bits
    }
}

#endif /* SM_HAL_SPI_SW_H_INCLUDED */
