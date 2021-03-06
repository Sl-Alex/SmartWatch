#include <cstring>
#include "sm_hw_powermgr.h"
#include "sm_display.h"

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

#define EVENTS_RISING (\
  (1UL << PIN_ALARM))

#define EVENTS_FALLING (\
  (1UL << PIN_S1) | \
  (1UL << PIN_S2) | \
  (1UL << PIN_S3) | \
  (1UL << PIN_S4) | \
  (1UL << PIN_AIRQ1) | \
  (1UL << PIN_BTRX))

#define EVENTS_COMMON (EVENTS_RISING | EVENTS_FALLING)

void SmHwPowerMgr::init(void)
{
    // Initialize GPIO EXTI
    // Pins 0 to 3 (any port)
    AFIO->EXTICR[0] = (PORT_S2    << ((PIN_S2    & 0x03) << 2)) |
                      (PORT_S3    << ((PIN_S3    & 0x03) << 2)) |
                      (PORT_AIRQ1 << ((PIN_AIRQ1 & 0x03) << 2));
                      //(PORT_ARDY  << ((PIN_ARDY  & 0x03) << 2));
    // Pins 4 to 7 (any port)
    AFIO->EXTICR[1] = (PORT_S4    << ((PIN_S4    & 0x03) << 2));
                      //(PORT_AIRQ3 << ((PIN_AIRQ3 & 0x03) << 2));
    // Pins 8 to 11 (any port)
    AFIO->EXTICR[2] = (PORT_BTRX  << ((PIN_BTRX  & 0x03) << 2));
//    AFIO->EXTICR[2] = (PORT_S1    << ((PIN_S1    & 0x03) << 2)) |
//                      (PORT_BTRX  << ((PIN_BTRX  & 0x03) << 2));
    // Pins 12 to 15 (any port)
    AFIO->EXTICR[3] = (PORT_S1    << ((PIN_S1    & 0x03) << 2));

    // Disable interrupts
    EXTI->IMR &= ~EVENTS_COMMON;
    // Disable events
    EXTI->EMR &= ~EVENTS_COMMON;

    // Set rising edges
    EXTI->RTSR &= ~EVENTS_COMMON;
    EXTI->RTSR |=  EVENTS_RISING;

    // Set falling edges
    EXTI->FTSR &= ~EVENTS_COMMON;
    EXTI->FTSR |=  EVENTS_FALLING;

    // Enable events
    EXTI->EMR =  EVENTS_COMMON;
    // Enable interrupts
//    EXTI->IMR |= (1UL << PIN_ALARM);
    EXTI->IMR = EVENTS_COMMON; // Enable all

    // Clear pending events (by writing one)
    EXTI->PR |= EVENTS_COMMON;

    sleepBlockers = 0;
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

bool SmHwPowerMgr::subscribe(SmHwPowerMgrIface *iface)
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

#pragma GCC push_options
#pragma GCC optimize ("O0")
void SmHwPowerMgr::sleep(void)
{
    __disable_irq();

    // Call onSleep() for all subscribers
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface != 0)
        {
            mPool[i].iface->onSleep();
        }
    }

    EXTI->IMR = EVENTS_COMMON;
    EXTI->EMR = EVENTS_COMMON;

    // Clear pending events (by writing one)
    EXTI->PR  = 0xFFFFUL;

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

    uint32_t wakeSource;

    do
    {
        PWR->CR = tmpreg;
        // Set SLEEPDEEP bit of Cortex System Control Register
        SCB->SCR |= SCB_SCR_SLEEPDEEP;

        // Goes to sleep at this step
        __WFE();

        // Reset SLEEPDEEP bit of Cortex System Control Register
        SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);

        // Read all pending interrupts
        wakeSource = EXTI->PR;
        wakeSource &= EVENTS_COMMON;
    }
    while (wakeSource == 0);

    // All interrupts except of alarm
    const uint32_t isrClearMask = EVENTS_COMMON & (~(1UL << PIN_ALARM));
    // Disable all except of alarm
    EXTI->IMR &=~isrClearMask;
    // Clear all except of alarm
    EXTI->PR = isrClearMask;

    SystemInit();

    // Call onWake() for all subscribers
    for (uint32_t i = 0; i < mPoolSize; ++i)
    {
        if (mPool[i].iface != 0)
        {
            mPool[i].iface->onWake(wakeSource);
        }
    }
    allowSleep(SM_HW_SLEEPBLOCKER_BT, SM_HW_SLEEP_SHORT);
    //SmDisplay::getInstance()->powerOn();

    __enable_irq();
}
#pragma GCC pop_options

void SmHwPowerMgr::updateState(void)
{
    if (!canSleep)
        return;

    //return;
    sleep();
}

void SmHwPowerMgr::onTimer(uint32_t timeStamp)
{
    if (sleepBlockers == 0)
        canSleep = true;
}

void SmHwPowerMgr::blockSleep(SleepBlocker blocker)
{
    sleepBlockers |= blocker;
    canSleep = false;
}

void SmHwPowerMgr::allowSleep(SleepBlocker blocker, uint32_t timeout)
{
    // Remove blocker
    sleepBlockers &= ~blocker;
    // Subscribe for a notification and block sleep
    SmHalSysTimer::subscribe(this, timeout,false);
    canSleep = false;
}
