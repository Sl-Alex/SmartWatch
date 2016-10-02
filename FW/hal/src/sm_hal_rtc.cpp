#include "sm_hal_rtc.h"

#ifndef PC_SOFTWARE
#include "sm_hal_rcc.h"
/// @brief Just a magic 16-bit number. Stored in backup register, helps to determine
/// if RTC module has already been configured.
/// @note Can be any value except of zero
#define BKP_DR1             ((uint16_t)0x0004)
#define USR_RTC_CONFIGURED  0xABCD
/* --------- PWR registers bit address in the alias region ---------- */
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of DBP bit */
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define DBP_BitNumber            0x08
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
/* Alias word address of RTCEN bit */
#define BDCR_OFFSET               (RCC_OFFSET + 0x20)
/* BDCR register base address */
#define BDCR_ADDRESS              (PERIPH_BASE + BDCR_OFFSET)
#define RCC_LSE_OFF                      ((uint8_t)0x00)
#define RCC_LSE_ON                       ((uint8_t)0x01)
#define RCC_RTCCLKSource_LSE             ((uint32_t)0x00000100)
#define RTCEN_BitNumber           0x0F
#define BDCR_RTCEN_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BitNumber * 4))
#define RTC_FLAG_RSF         ((uint16_t)0x0008)  /*!< Registers Synchronized flag */
#define RTC_FLAG_RTOFF       ((uint16_t)0x0020)  /*!< RTC Operation OFF flag */
/* Alias word address of BDRST bit */
#define BDRST_BitNumber           0x10
#define BDCR_BDRST_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (BDRST_BitNumber * 4))
#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF)  /*!< RTC LSB Mask */
#define PRLH_MSB_MASK    ((uint32_t)0x000F0000)  /*!< RTC Prescaler MSB Mask */
#endif

#ifdef PC_SOFTWARE
#include <ctime>
#endif

#ifndef PC_SOFTWARE
inline void RTC_WaitForLastTask(void)
{
  /* Loop until RTOFF flag is set */
  while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET)
  {
  }
}
#endif

void SmHalRtc::init(void)
{
#ifndef PC_SOFTWARE

    // Enable RTC interrupts
    NVIC->ISER[0] = NVIC_ISER_SETENA_5;

    uint16_t state = (*(__IO uint16_t *)(BKP_BASE + BKP_DR1));

    if (state == USR_RTC_CONFIGURED)
    {
        // Enable PWR and BKP clocks
        SmHalRcc::clockEnable(RCC_PERIPH_PWR);
        SmHalRcc::clockEnable(RCC_PERIPH_BKP);

        /* Allow access to BKP Domain */
        *(__IO uint32_t *) CR_DBP_BB = ENABLE;

        /* Enable LSE */
        *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;
        *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;
        *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_ON;

        /* Wait till LSE is ready */
        uint32_t tmp;
        while (tmp == 0)
        {
            tmp = RCC->BDCR;
            tmp &= 0x02;
        }

        /* Select LSE as RTC Clock Source */
        RCC->BDCR |= RCC_RTCCLKSource_LSE;

        /* Enable RTC Clock */
        *(__IO uint32_t *) BDCR_RTCEN_BB = 1;

        /* Wait for RTC registers synchronization */
        /* Clear RSF flag */
        RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;
        /* Loop until RSF flag is set */
        while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET)
        {
        }

        /* Enable the RTC Second */
        //RTC_ITConfig(RTC_IT_SEC, ENABLE);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        //taskEXIT_CRITICAL();
        return;
    }

    /* Enable PWR and BKP clocks */
    SmHalRcc::clockEnable(RCC_PERIPH_PWR);
    SmHalRcc::clockEnable(RCC_PERIPH_BKP);

    /* Allow access to BKP Domain */
    *(__IO uint32_t *) CR_DBP_BB = ENABLE;

    /* Reset Backup Domain */
    *(__IO uint32_t *) BDCR_BDRST_BB = 1;
    *(__IO uint32_t *) BDCR_BDRST_BB = 0;

    /* Enable LSE */
    *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;
    *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_OFF;
    *(__IO uint8_t *) BDCR_ADDRESS = RCC_LSE_ON;

    /* Wait till LSE is ready */
    uint32_t tmp;
    while (tmp == 0)
    {
        tmp = RCC->BDCR;
        tmp &= 0x02;
    }

    /* Select LSE as RTC Clock Source */
    RCC->BDCR |= RCC_RTCCLKSource_LSE;

    /* Enable RTC Clock */
    *(__IO uint32_t *) BDCR_RTCEN_BB = 1;

    /* Wait for RTC registers synchronization */
    /* Clear RSF flag */
    RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;
    /* Loop until RSF flag is set */
    while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET)
    {
    }

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    //RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC->CRL |= RTC_CRL_CNF;
    /* Set RTC PRESCALER MSB word */
    RTC->PRLH = (32767 & PRLH_MSB_MASK) >> 16;
    /* Set RTC PRESCALER LSB word */
    RTC->PRLL = (32767 & RTC_LSB_MASK);
    RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    (*(__IO uint16_t *)(BKP_BASE + BKP_DR1)) = USR_RTC_CONFIGURED;

#endif
}

uint32_t SmHalRtc::getCounter(void)
{
#ifdef PC_SOFTWARE
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    return (now->tm_hour*3600 + now->tm_min*60 + now->tm_sec);
#else
    uint32_t tmp = RTC->CNTL;
    return (((uint32_t)RTC->CNTH << 16 ) | tmp) ;
#endif
}

void SmHalRtc::setCounter(uint32_t value)
{
#ifndef PC_SOFTWARE
    /* Set the CNF flag to enter in the Configuration Mode */
    RTC->CRL |= RTC_CRL_CNF;

    /* Set RTC COUNTER MSB word */
    RTC->CNTH = value >> 16;
    /* Set RTC COUNTER LSB word */
    RTC->CNTL = (value & RTC_LSB_MASK);

    /* Reset the CNF flag to exit from the Configuration Mode */
    RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF);
#endif
}


/*
extern "C" void SysTick_Handler(void)
{
    SmHalSysTimer::mTimeStamp++;
}
*/
