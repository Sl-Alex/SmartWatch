#include <cstring>
#include "sm_hw_powermgr.h"

#define PWR_Regulator_ON          ((uint32_t)0x00000000)
#define PWR_Regulator_LowPower    ((uint32_t)0x00000001)

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)

#define PORT_S1    0UL  // GPIOA
#define PORT_S2    1UL  // GPIOB
#define PORT_S3    0UL  // GPIOA
#define PORT_S4    0UL  // GPIOA
#define PORT_BTST  0UL  // GPIOA
#define PORT_BTRX  0UL  // GPIOA
#define PORT_AIRQ1 1UL  // GPIOB
#define PORT_AIRQ2 1UL  // GPIOB
#define PORT_AIRQ3 0UL  // GPIOA
#define PORT_ARDY  1UL  // GPIOB

#define PIN_S1     12UL
#define PIN_S2     3UL
#define PIN_S3     1UL
#define PIN_S4     6UL
#define PIN_BTST   3UL  // BT status
#define PIN_BTRX   10UL // BT RX
#define PIN_AIRQ1  2UL
#define PIN_AIRQ2  1UL
#define PIN_AIRQ3  7UL
#define PIN_ARDY   0UL

// Potential EXTI conflicts:
// S2 <=> BTST  ==> Both signals use pin1, BTRX can be used for waking instead of BT
// S3 <=> AI2   ==> AI2 can be disabled. @todo: Check BMC150 datasheet
// So, the following pins can wake it up:
// S1      A  12
// S2      B  3
// S3      A  1
// S4      A  6
// BTRX    A  10
// AI1     B  2
// AI3     A  7
// ARDY    B  0

#define EXTI_LINES (\
  (1UL << PIN_S1) | \
  (1UL << PIN_S2) | \
  (1UL << PIN_S3) | \
  (1UL << PIN_S4) | \
  (1UL << PIN_BTRX) | \
  (1UL << PIN_AIRQ1) | \
  (1UL << PIN_AIRQ3) | \
  (1UL << PIN_ARDY))

/// @todo Check with the hardware
void SmHwPowerMgr::init(void)
{
    mPool = 0;
    mPoolSize = 0;

    // Initialize GPIO EXTI
    // Pins 0 to 3 (any port)
    AFIO->EXTICR[0] = (PORT_S2    << ((PIN_S2    & 0x03) << 2)) | \
                      (PORT_S3    << ((PIN_S3    & 0x03) << 2)) | \
                      (PORT_AIRQ1 << ((PIN_AIRQ1 & 0x03) << 2)) | \
                      (PORT_ARDY  << ((PIN_ARDY  & 0x03) << 2));
    // Pins 4 to 7 (any port)
    AFIO->EXTICR[1] = (PORT_S4    << ((PIN_S4    & 0x03) << 2)) | \
                      (PORT_AIRQ3 << ((PIN_AIRQ3 & 0x03) << 2));
    // Pins 8 to 11 (any port)
    AFIO->EXTICR[2] = (PORT_S1    << ((PIN_S1    & 0x03) << 2)) | \
                      (PORT_BTRX  << ((PIN_BTRX  & 0x03) << 2));
    // Pins 12 to 15 (any port)
    AFIO->EXTICR[3] = 0;

    // Initialize EXTI events
    // Add to event mode register
    EXTI->IMR &= ~EXTI_LINES;
    EXTI->EMR |= EXTI_LINES;
    // Set falling edge
    EXTI->RTSR &= ~EXTI_LINES;
    EXTI->FTSR |= EXTI_LINES;
}

void SmHwPowerMgr::initSubscribersPool(uint8_t max)
{
    if (mPool)
        deinitSubscribersPool();

    mPoolSize = max;
    mPool = new SmHwPowerMgrSubscriber[mPoolSize];
    memset(mPool, 0, sizeof(SmHwPowerMgrSubscriber) * mPoolSize);
}

void SmHwPowerMgr::deinitSubscribersPool(void)
{
    if (mPool)
        delete[] mPool;

    mPoolSize = 0;
    mPool = 0;
}

bool SmHwPowerMgr::subscribe(SmHwPowerMgrIface *iface, uint32_t period, bool repeat)
{
    // Search for existing, update if found
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface == iface)
        {
            return true;
        }
    }
    // Subscribe new client
    for (uint32_t i = 0; i < mPoolSize; i++)
    {
        if (mPool[i].iface == 0)
        {
            mPool[i].iface = iface;
            return true;
        }
    }
    // Can not update existing or create new
    return false;
}

void SmHwPowerMgr::unsubscribe(SmHwPowerMgrIface *iface)
{
    // Search for existing, remove if found
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface == iface)
        {
            mPool[i].iface = 0;
            break;
        }
    }
}

void SmHwPowerMgr::sleep(void)
{
    // Call onSleep() for all subscribers
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface != 0)
        {
            mPool[i].iface->onSleep();
        }
    }

    // Entering STOP state with low power regulator mode and WFE
    uint32_t tmpreg = 0;

    // Select the regulator state in STOP mode
    tmpreg = PWR->CR;
    // Clear PDDS and LPDS bits
    tmpreg &= CR_DS_MASK;
    // Set LPDS bit according to PWR_Regulator value
    tmpreg |= PWR_Regulator_LowPower;
    // Store the new value
    PWR->CR = tmpreg;
    // Set SLEEPDEEP bit of Cortex System Control Register
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    // Goes to sleep at this step
    __WFE();

    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);

    // Call onWake() for all subscribers
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface != 0)
        {
            mPool[i].iface->onWake();
        }
    }
}
