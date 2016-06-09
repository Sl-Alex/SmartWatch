#ifndef SM_HW_RCC_H_INCLUDED
#define SM_HW_RCC_H_INCLUDED

#include "stm32f10x.h"

enum RccAhbPeriph
{

};

enum RccApb1Periph
{
    RCC_PERIPH_TIM2   = 0x00000001,
    RCC_PERIPH_TIM3   = 0x00000002,
    RCC_PERIPH_TIM4   = 0x00000004,
    RCC_PERIPH_TIM5   = 0x00000008,
    RCC_PERIPH_TIM6   = 0x00000010,
    RCC_PERIPH_TIM7   = 0x00000020,
    RCC_PERIPH_TIM12  = 0x00000040,
    RCC_PERIPH_TIM13  = 0x00000080,
    RCC_PERIPH_TIM14  = 0x00000100,
    RCC_PERIPH_WWDG   = 0x00000800,
    RCC_PERIPH_SPI2   = 0x00004000,
    RCC_PERIPH_SPI3   = 0x00008000,
    RCC_PERIPH_USART2 = 0x00020000,
    RCC_PERIPH_USART3 = 0x00040000,
    RCC_PERIPH_UART4  = 0x00080000,
    RCC_PERIPH_UART5  = 0x00100000,
    RCC_PERIPH_I2C1   = 0x00200000,
    RCC_PERIPH_I2C2   = 0x00400000,
    RCC_PERIPH_USB    = 0x00800000,
    RCC_PERIPH_CAN1   = 0x02000000,
    RCC_PERIPH_CAN2   = 0x04000000,
    RCC_PERIPH_BKP    = 0x08000000,
    RCC_PERIPH_PWR    = 0x10000000,
    RCC_PERIPH_DAC    = 0x20000000,
    RCC_PERIPH_CEC    = 0x40000000
};

enum RccApb2Periph
{
    RCC_PERIPH_AFIO   = 0x00000001UL,
    RCC_PERIPH_GPIOA  = 0x00000004UL,
    RCC_PERIPH_GPIOB  = 0x00000008UL,
    RCC_PERIPH_GPIOC  = 0x00000010UL,
    RCC_PERIPH_GPIOD  = 0x00000020UL,
    RCC_PERIPH_GPIOE  = 0x00000040UL,
    RCC_PERIPH_GPIOF  = 0x00000080UL,
    RCC_PERIPH_GPIOG  = 0x00000100UL,
    RCC_PERIPH_ADC1   = 0x00000200UL,
    RCC_PERIPH_ADC2   = 0x00000400UL,
    RCC_PERIPH_TIM1   = 0x00000800UL,
    RCC_PERIPH_SPI1   = 0x00001000UL,
    RCC_PERIPH_TIM8   = 0x00002000UL,
    RCC_PERIPH_USART1 = 0x00004000UL,
    RCC_PERIPH_ADC3   = 0x00008000UL,
    RCC_PERIPH_TIM15  = 0x00010000UL,
    RCC_PERIPH_TIM16  = 0x00020000UL,
    RCC_PERIPH_TIM17  = 0x00040000UL,
    RCC_PERIPH_TIM9   = 0x00080000UL,
    RCC_PERIPH_TIM10  = 0x00100000UL,
    RCC_PERIPH_TIM11  = 0x00200000UL
};

class SmHwRcc
{
public:
	static void RccClockEnable(RccAhbPeriph periph)
	{
		RCC->AHBENR |= (uint32_t)periph;
	}
	static void RccClockEnable(RccApb1Periph periph)
	{
		RCC->APB1ENR |= (uint32_t)periph;
	}
	static void RccClockEnable(RccApb2Periph periph)
	{
		RCC->APB2ENR |= (uint32_t)periph;
	}

	static void RccClockDisable(RccAhbPeriph periph)
	{
		RCC->AHBENR &= ~(uint32_t)periph;
	}
	static void RccClockDisable(RccApb1Periph periph)
	{
		RCC->APB1ENR &= ~(uint32_t)periph;
	}
	static void RccClockDisable(RccApb2Periph periph)
	{
		RCC->APB2ENR &= ~(uint32_t)periph;
	}
};

#endif /* SM_HW_RCC_H_INCLUDED */
