#include "sm_hal_rcc.h"

RCC_Clocks SmHalRcc::clocks;

void SmHalRcc::clockEnable(RccAhbPeriph periph)
{
    RCC->AHBENR |= (uint32_t)periph;
}

void SmHalRcc::clockEnable(RccApb1Periph periph)
{
    RCC->APB1ENR |= (uint32_t)periph;
}

void SmHalRcc::clockEnable(RccApb2Periph periph)
{
    RCC->APB2ENR |= (uint32_t)periph;
}

void SmHalRcc::clockDisable(RccAhbPeriph periph)
{
    RCC->AHBENR &= ~(uint32_t)periph;
}

void SmHalRcc::clockDisable(RccApb1Periph periph)
{
    RCC->APB1ENR &= ~(uint32_t)periph;
}

void SmHalRcc::clockDisable(RccApb2Periph periph)
{
    RCC->APB2ENR &= ~(uint32_t)periph;
}

#ifdef STM32F10X_CL
void SmHalRcc::reset(RccAhbPeriph periph)
{
    RCC->AHBRSTR |= periph;
    RCC->AHBRSTR &=~periph;
}
#endif /* STM32F10X_CL */

void SmHalRcc::reset(RccApb1Periph periph)
{
    RCC->APB1RSTR |= periph;
    RCC->APB1RSTR &=~periph;
}

void SmHalRcc::reset(RccApb2Periph periph)
{
    RCC->APB2RSTR |= periph;
    RCC->APB2RSTR &=~periph;
}


/**
  * @brief  Returns the frequencies of different on chip clocks.
  * @param  RCC_Clocks: pointer to a RCC_ClocksTypeDef structure which will hold
  *         the clocks frequencies.
  * @note   The result of this function could be not correct when using
  *         fractional value for HSE crystal.
  * @retval None
  */
void SmHalRcc::updateClocks(void)
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0, presc = 0;

#ifdef  STM32F10X_CL
    uint32_t prediv1source = 0, prediv1factor = 0, prediv2factor = 0, pll2mull = 0;
#endif /* STM32F10X_CL */

#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
    uint32_t prediv1factor = 0;
#endif

    /* Get SYSCLK source -------------------------------------------------------*/
#define CFGR_SWS_Mask             ((uint32_t)0x0000000C)
    tmp = RCC->CFGR & CFGR_SWS_Mask;

    switch (tmp)
    {
        case 0x00:  /* HSI used as system clock */
            clocks.SYSCLK_Frequency = HSI_VALUE;
            break;
        case 0x04:  /* HSE used as system clock */
            clocks.SYSCLK_Frequency = HSE_VALUE;
            break;
        case 0x08:  /* PLL used as system clock */
            /* Get PLL clock source and multiplication factor ----------------------*/
            #define CFGR_PLLMull_Mask         ((uint32_t)0x003C0000)
            pllmull = RCC->CFGR & CFGR_PLLMull_Mask;
            #define CFGR_PLLSRC_Mask          ((uint32_t)0x00010000)
            pllsource = RCC->CFGR & CFGR_PLLSRC_Mask;

            #ifndef STM32F10X_CL
            pllmull = ( pllmull >> 18) + 2;

            if (pllsource == 0x00)
            {
                /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                clocks.SYSCLK_Frequency = (HSI_VALUE >> 1) * pllmull;
            }
            else
            {
                #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
                prediv1factor = (RCC->CFGR2 & CFGR2_PREDIV1) + 1;
                /* HSE oscillator clock selected as PREDIV1 clock entry */
                clocks.SYSCLK_Frequency = (HSE_VALUE / prediv1factor) * pllmull;
                #else
                /* HSE selected as PLL clock entry */
                #define CFGR_PLLXTPRE_Mask        ((uint32_t)0x00020000)
                if ((RCC->CFGR & CFGR_PLLXTPRE_Mask) != (uint32_t)RESET)
                {/* HSE oscillator clock divided by 2 */
                    clocks.SYSCLK_Frequency = (HSE_VALUE >> 1) * pllmull;
                }
                else
                {
                    clocks.SYSCLK_Frequency = HSE_VALUE * pllmull;
                }
                #endif
            }
            #else
            pllmull = pllmull >> 18;

            if (pllmull != 0x0D)
            {
                pllmull += 2;
            }
            else
            {
                /* PLL multiplication factor = PLL input clock * 6.5 */
                pllmull = 13 / 2;
            }

            if (pllsource == 0x00)
            {
                /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                RCC_Clocks->SYSCLK_Frequency = (HSI_VALUE >> 1) * pllmull;
            }
            else
            {
                /* PREDIV1 selected as PLL clock entry */
                /* Get PREDIV1 clock source and division factor */
                prediv1source = RCC->CFGR2 & CFGR2_PREDIV1SRC;
                prediv1factor = (RCC->CFGR2 & CFGR2_PREDIV1) + 1;

                if (prediv1source == 0)
                {
                    /* HSE oscillator clock selected as PREDIV1 clock entry */
                    clocks.SYSCLK_Frequency = (HSE_VALUE / prediv1factor) * pllmull;
                }
                else
                {
                    /* PLL2 clock selected as PREDIV1 clock entry */

                    /* Get PREDIV2 division factor and PLL2 multiplication factor */
                    prediv2factor = ((RCC->CFGR2 & CFGR2_PREDIV2) >> 4) + 1;
                    pll2mull = ((RCC->CFGR2 & CFGR2_PLL2MUL) >> 8 ) + 2;
                    RCC_Clocks->SYSCLK_Frequency = (((HSE_VALUE / prediv2factor) * pll2mull) / prediv1factor) * pllmull;
                }
            }
            #endif /* STM32F10X_CL */
            break;
        default:
            clocks.SYSCLK_Frequency = HSI_VALUE;
            break;
    }

    /* Compute HCLK, PCLK1, PCLK2 and ADCCLK clocks frequencies ----------------*/

    /* Get HCLK prescaler */
    #define CFGR_HPRE_Set_Mask        ((uint32_t)0x000000F0)
    tmp = RCC->CFGR & CFGR_HPRE_Set_Mask;
    tmp = tmp >> 4;
    static __I uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};
    presc = APBAHBPrescTable[tmp];

    /* HCLK clock frequency */
    clocks.HCLK_Frequency = clocks.SYSCLK_Frequency >> presc;

    /* Get PCLK1 prescaler */
    #define CFGR_PPRE1_Set_Mask       ((uint32_t)0x00000700)
    tmp = RCC->CFGR & CFGR_PPRE1_Set_Mask;
    tmp = tmp >> 8;
    presc = APBAHBPrescTable[tmp];

    /* PCLK1 clock frequency */
    clocks.PCLK1_Frequency = clocks.HCLK_Frequency >> presc;

    /* Get PCLK2 prescaler */
    #define CFGR_PPRE2_Set_Mask       ((uint32_t)0x00003800)
    tmp = RCC->CFGR & CFGR_PPRE2_Set_Mask;
    tmp = tmp >> 11;
    presc = APBAHBPrescTable[tmp];

    /* PCLK2 clock frequency */
    clocks.PCLK2_Frequency = clocks.HCLK_Frequency >> presc;

    /* Get ADCCLK prescaler */
    #define CFGR_ADCPRE_Set_Mask      ((uint32_t)0x0000C000)
    tmp = RCC->CFGR & CFGR_ADCPRE_Set_Mask;
    tmp = tmp >> 14;
    static __I uint8_t ADCPrescTable[4] = {2, 4, 6, 8};
    presc = ADCPrescTable[tmp];

    /* ADCCLK clock frequency */
    clocks.ADCCLK_Frequency = clocks.PCLK2_Frequency / presc;
}

const RCC_Clocks *const SmHalRcc::getClocks(void)
{
    return &clocks;
}
