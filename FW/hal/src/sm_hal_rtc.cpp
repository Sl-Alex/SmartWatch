#include "sm_hal_rtc.h"
#include <utility>

#ifdef PC_SOFTWARE
#include <ctime>
#include <qglobal.h>
    static uint32_t EMUL_COUNTER = 0;
    static uint16_t BKP_YEAR = 0;
    static uint16_t BKP_MONTH = 0;
    static uint16_t BKP_DAY = 0;
#else

#include "sm_hal_rcc.h"
/// @brief Just a magic 16-bit number. Stored in backup register, helps to determine
/// if some external or internal module has already been configured.
/// @note Can be any value except of zero
#define USR_STATE_CONFIGURED  0xABCD

#define BKP_DR1             ((uint16_t)0x0004)
#define BKP_DR2             ((uint16_t)0x0008)
#define BKP_DR3             ((uint16_t)0x000C)
#define BKP_DR4             ((uint16_t)0x0010)
#define BKP_DR5             ((uint16_t)0x0014)
#define BKP_DR6             ((uint16_t)0x0018)
#define BKP_DR7             ((uint16_t)0x001C)
#define BKP_DR8             ((uint16_t)0x0020)
#define BKP_DR9             ((uint16_t)0x0024)

#define BKP_CONFIGURED      (*(__IO uint16_t *)(BKP_BASE + BKP_DR1))
#define BKP_YEAR            (*(__IO uint16_t *)(BKP_BASE + BKP_DR2))
#define BKP_MONTH           (*(__IO uint16_t *)(BKP_BASE + BKP_DR3))
#define BKP_DAY             (*(__IO uint16_t *)(BKP_BASE + BKP_DR4))
#define BKP_HM10_PRECONF    (*(__IO uint16_t *)(BKP_BASE + BKP_DR5))
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

#ifndef PC_SOFTWARE
inline void RTC_WaitForLastTask(void)
{
    /* Loop until RTOFF flag is set */
    while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET){}
}

void RTC_WaitForSynchro(void)
{
    /* Clear RSF flag */
    RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;
    /* Loop until RSF flag is set */
    while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET){}
}

inline void RTC_EnterConfigMode(void)
{
    /* Set the CNF flag to enter in the Configuration Mode */
    RTC->CRL |= RTC_CRL_CNF;
}

inline void RTC_ExitConfigMode(void)
{
    /* Reset the CNF flag to exit from the Configuration Mode */
    RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF);
}
#endif

void SmHalRtc::init(void)
{
#ifdef PC_SOFTWARE
    SmHalRtcTime rtc;

    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    rtc.year = now->tm_year + 1900;
    rtc.month = now->tm_mon + 1;
    rtc.day = now->tm_mday;
    rtc.hour = now->tm_hour;
    rtc.minute = now->tm_min;
    rtc.second = now->tm_sec;
    setDateTime(rtc);
#else

    uint16_t state = BKP_CONFIGURED;

    if (state == USR_STATE_CONFIGURED)
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

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
    else
    {
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

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        /* Set RTC prescaler: set RTC period to 1sec */
        RTC_EnterConfigMode();
        /* Set RTC PRESCALER MSB word */
        RTC->PRLH = (32767 & PRLH_MSB_MASK) >> 16;
        /* Set RTC PRESCALER LSB word */
        RTC->PRLL = (32767 & RTC_LSB_MASK);
        RTC_ExitConfigMode();

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        BKP_CONFIGURED = USR_STATE_CONFIGURED;
    }

    // Enable RTC interrupts
    NVIC_EnableIRQ(RTCAlarm_IRQn);
    SmHwPowerMgr::getInstance()->subscribe(this);
#endif
}

uint32_t SmHalRtc::getCounter(void)
{
#ifdef PC_SOFTWARE
    return EMUL_COUNTER;
#else
    RTC_WaitForSynchro();
    uint32_t tmp = RTC->CNTL;
    return (((uint32_t)RTC->CNTH << 16 ) | tmp) ;
    RTC_WaitForSynchro();
#endif
}

uint32_t SmHalRtc::getAlarm(void)
{
#ifndef PC_SOFTWARE
    uint32_t tmp = RTC->ALRL;
    return (((uint32_t)RTC->ALRH << 16 ) | tmp) ;
#else
    return 0;
#endif
}

void SmHalRtc::setCounter(uint32_t value)
{
#ifdef PC_SOFTWARE
    EMUL_COUNTER = value;
#else
    RTC_WaitForLastTask();
    RTC_EnterConfigMode();

    /* Set RTC COUNTER MSB word */
    RTC->CNTH = value >> 16;
    /* Set RTC COUNTER LSB word */
    RTC->CNTL = (value & RTC_LSB_MASK);

    RTC_ExitConfigMode();
    RTC_WaitForLastTask();
#endif
}

#ifdef PC_SOFTWARE
void SmHalRtc::incrementCounter()
{
    EMUL_COUNTER++;
    if (EMUL_COUNTER >= 24*3600)
    {
        EMUL_COUNTER %= 24*3600;
        RTCAlarm_IRQHandler();
    }
}
#endif

void SmHalRtc::setAlarm(uint32_t value)
{
#ifdef PC_SOFTWARE
    Q_UNUSED (value)
#else
    RTC_WaitForLastTask();
    RTC_EnterConfigMode();

    // Set ALARM value
    RTC->ALRH = value >> 16;
    RTC->ALRL = (value & RTC_LSB_MASK);
    // Clear alarm flag
    RTC->CRL &=~RTC_CRL_ALRF;
    // Enable alarm interrupts
    RTC->CRH |= RTC_CRH_ALRIE;

    RTC_ExitConfigMode();
    RTC_WaitForLastTask();
#endif
}

void SmHalRtc::setDateTime(SmHalRtcTime &time)
{
    setDate(time);
    setTime(time);
#ifndef PC_SOFTWARE
    updateAlarm();
#endif
}

SmHalRtc::SmHalRtcTime SmHalRtc::getDateTime(void)
{
    SmHalRtcTime rtc;

    rtc.year  = BKP_YEAR;
    rtc.month = BKP_MONTH;
    rtc.day   = BKP_DAY;

    uint32_t cnt = getCounter();
    rtc.hour = (cnt / 3600) % 24;
    uint32_t rest = cnt % 3600;
    rtc.minute = rest / 60;
    rtc.second = rest % 60;

    return std::move(rtc);
}

void SmHalRtc::setDate(SmHalRtcTime &time)
{
    BKP_YEAR  = time.year;
    BKP_MONTH = time.month;
    BKP_DAY   = time.day;
}

void SmHalRtc::setTime(SmHalRtcTime &time)
{
    uint32_t cnt = time.hour * 3600 +
                   time.minute * 60 +
                   time.second;
    setCounter(cnt);
}

#ifndef PC_SOFTWARE
void SmHalRtc::onSleep(void)
{
}

void SmHalRtc::onWake(uint32_t wakeSource)
{
    // After wake up RCT CR should be reconfigured ()
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
    RTC_EnterConfigMode();

    // Re-enable alarm interrupts
    RTC->CRH |= RTC_CRH_ALRIE;

    RTC_ExitConfigMode();
    RTC_WaitForLastTask();
//    NVIC_EnableIRQ(RTCAlarm_IRQn);
}

void SmHalRtc::updateAlarm(void)
{
    uint32_t days = getCounter();
    days = days / (3600 * 24);
    setAlarm((days+1) * (3600 * 24) - 1);
    RTC_WaitForLastTask();
}

#endif

extern "C" void RTCAlarm_IRQHandler(void)
{
#ifndef PC_SOFTWARE
    SmHalRtc::getInstance()->updateAlarm();
#endif

    BKP_DAY++;
    if (BKP_DAY > SmHalRtc::getInstance()->getDaysOfMonth(BKP_YEAR, BKP_MONTH))
    {
        BKP_DAY = 1;
        BKP_MONTH++;
        if (BKP_MONTH > 12)
        {
            BKP_MONTH = 1;
            BKP_YEAR++;
        }
    }

#ifndef PC_SOFTWARE
    // Clear interrupt flag
    EXTI->PR = (1UL << 17UL);
#endif
}

//-----------------------------------------------------------------------
// Date/time utils
//-----------------------------------------------------------------------

uint32_t SmHalRtc::getDayOfWeek(SmHalRtcTime &time)
{
    uint32_t a1 = (14 - time.month) / 12;
    uint32_t a2 = time.year - a1;
    uint32_t a3 = time.month + 12 * a1 - 2;

    return (time.day + a2 + a2 / 4 - a2 / 100 + a2 / 400 + (31 * a3) / 12) % 7;
}

uint32_t SmHalRtc::isLeap(uint32_t year)
{
    if (year % 100)
    {
        if (year % 400)
            return 1;
        else
            return 0;
    }
    else
    {
        if (year % 4)
            return 1;
        else
            return 0;
    }
}

uint32_t SmHalRtc::getDaysOfMonth(uint32_t year, uint32_t month)
{
    if ((month ==  1) || (month ==  3) || (month ==  5) || (month ==  7) || // Jan, Mar, May, Jul
        (month ==  8) || (month == 10) || (month == 12))                    // Aug, Oct, Dec
        return 31;
    if ((month ==  4) || (month ==  6) || (month ==  9) || (month == 11))   // Apr, Jun, Sep, Nov
        return 30;
    if (isLeap(year)) // Feb
        return 29;
    else
        return 28;
}

bool SmHalRtc::isHm10Preconf(void)
{
#ifndef PC_SOFTWARE
    return (BKP_HM10_PRECONF == USR_STATE_CONFIGURED);
#else
    return false;
#endif
}

void SmHalRtc::setHm10Preconf(void)
{
#ifndef PC_SOFTWARE
    BKP_HM10_PRECONF = USR_STATE_CONFIGURED;
#endif
}
