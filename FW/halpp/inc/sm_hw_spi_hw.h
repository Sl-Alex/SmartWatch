#ifndef SM_HW_SPI_HW_H_INCLUDED
#define SM_HW_SPI_HW_H_INCLUDED

#include "sm_hw_abstract_spi.h"
#include "sm_hw_abstract_gpio.h"

template<uint32_t SPI_BASE, SpiConfig CFG>
class SmHwSpiHw: public SmHwAbstractSpi
{
public:
	virtual void init(SpiMode mode, SpiWidth width);
//	virtual void setSs(int ss) {SmHwAbstractSpi}
//	virtual void resetSs(int ss);
	virtual void transfer(void * in, void * out, int size);
};

template<uint32_t SPI_BASE, SpiConfig CFG>
void SmHwSpiHw<SPI_BASE, CFG>::init(SpiMode mode, SpiWidth width)
{
    uint16_t tmpreg;

	/* Get the SPIx CR1 value */
	tmpreg = ((SPI_TypeDef *)SPI_BASE)->CR1;
	/* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
	tmpreg &= CR1_CLEAR_Mask;
	/* Configure SPIx: direction, NSS management, first transmitted bit, BaudRate prescaler
	master/salve mode, CPOL and CPHA */
	/* Set BIDImode, BIDIOE and RxONLY bits according to SPI_Direction value */
	/* Set SSM, SSI and MSTR bits according to SPI_Mode and SPI_NSS values */
	/* Set LSBFirst bit according to SPI_FirstBit value */
	/* Set BR bits according to SPI_BaudRatePrescaler value */
	/* Set CPOL bit according to SPI_CPOL value */
	/* Set CPHA bit according to SPI_CPHA value */
	tmpreg |= (CFG | SPI_HW_SPI_MSMODE_MASTER |
	width | mode | SPI_HW_SPI_NSS_MODE_SOFT);

	/* Write to SPIx CR1 */
	((SPI_TypeDef *)SPI_BASE)->CR1 = tmpreg;

	/* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
#define SPI_Mode_Select      ((uint16_t)0xF7FF)
#define I2S_Mode_Select      ((uint16_t)0x0800)
	((SPI_TypeDef *)SPI_BASE)->I2SCFGR &= SPI_Mode_Select;

	/* Enable SPI */
#define CR1_SPE_Set          ((uint16_t)0x0040)
	((SPI_TypeDef *)SPI_BASE)->CR1 |= CR1_SPE_Set;
}

template<uint32_t SPI_BASE, SpiConfig CFG>
void SmHwSpiHw<SPI_BASE, CFG>::transfer(void * in, void * out, int size)
{
	uint8_t * pOut = (uint8_t *)out;
	uint8_t * pIn  = (uint8_t *)in;
	while (size--)
	{
		uint8_t data = *pOut;

		/*!< Loop while DR register in not emplty */
#define SPI_I2S_FLAG_TXE                ((uint16_t)0x0002)
		while ((((SPI_TypeDef *)SPI_BASE)->SR & SPI_I2S_FLAG_TXE) == 0);

		/*!< Send byte through the SPI peripheral */
		((SPI_TypeDef *)SPI_BASE)->DR = data;

        /*!< Wait to receive a byte */
#define SPI_I2S_FLAG_RXNE               ((uint16_t)0x0001)
		while ((((SPI_TypeDef *)SPI_BASE)->SR & SPI_I2S_FLAG_RXNE) == 0);

		data = ((SPI_TypeDef *)SPI_BASE)->DR;
		if (CFG == SM_HW_SPI_CFG_FULL_DUPLEX)
		{
			*pIn = data;
			pIn++;
		}
		pOut++;
	}
}
/*
template<uint32_t SPI_BASE, SpiConfig CFG>
void SmHwSpiHw<SPI_BASE, CFG>::setSs(int ss)
{
    SmHwAbstractSpi::setSs(ss);
}

template<uint32_t SPI_BASE, SpiConfig CFG>
void SmHwSpiHw<SPI_BASE, CFG>::resetSs(int ss)
{
    SmHwAbstractSpi::resetSs(ss);
}
*/

#endif /* SM_HW_SPI_HW_H_INCLUDED */
