#include "sm_hw_battery.h"

#include "stm32f10x.h"

void SmHwBattery::init()
{
    ADC1->SQR3 = 0;

    ADC1->CR2 = ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG;

    ADC1->CR2 |= ADC_CR2_ADON;

    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_RSTCAL)
    {
    }

    ADC1->CR2 |= ADC_CR2_CAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_CAL)
    {
    }
}

uint32_t SmHwBattery::readValue(void)
{
    uint32_t val = 0;
	ADC1->CR2 |= ADC_CR2_SWSTART;
	while ((ADC1->SR & ADC_SR_EOC) != ADC_SR_EOC)
	{

	}
	val = ADC1->DR;
	return val;
}
