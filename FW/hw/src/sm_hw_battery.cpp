#include "sm_hw_battery.h"

#include "stm32f10x.h"
#include "sm_hal_gpio.h"

#define GPIO_EN_PORT    GPIOA_BASE
#define GPIO_EN_PIN     2
#define MEAS_DELAY      2
#define MEAS_INTERVAL   10000

#define GPIO_AIN_PORT   GPIOA_BASE
#define GPIO_AIN_PIN    0

void SmHwBattery::init()
{
    mValue = 0;

    mGpioEn = new SmHalGpio<GPIO_EN_PORT, GPIO_EN_PIN>();
    mGpioEn->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);
    mGpioEn->setPin();

    SmHalGpio<GPIO_AIN_PORT, GPIO_AIN_PIN> BatGpio;
    BatGpio.setModeSpeed(SM_HAL_GPIO_MODE_AIN, SM_HAL_GPIO_SPEED_2M);

    ADC1->SQR3 = 0;

    ADC1->CR2 = ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG;

    ADC1->CR2 |= ADC_CR2_ADON;

    /// @todo Add several ms delay before ADC calibration
    uint32_t timestamp = SmHalSysTimer::getTimeStamp();
    timestamp += MEAS_DELAY;    // 2ms
    // Wait a bit before proceed
    while (SmHalSysTimer::getTimeStamp() <= timestamp);

    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_RSTCAL)
    {
    }

    ADC1->CR2 |= ADC_CR2_CAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_CAL)
    {
    }

    // Enable interrupts
    ADC1->CR1 |= ADC_CR1_EOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);

    // Start measurement
    ADC1->CR2 |= ADC_CR2_SWSTART;

    mMeasStep = 0;

    SmHwPowerMgr::getInstance()->subscribe(this);
}

void SmHwBattery::onTimer(uint32_t timeStamp)
{
    if (mMeasStep == 0)
    {
        mGpioEn->setPin();
        SmHalSysTimer::subscribe(this, MEAS_DELAY, false);
        mMeasStep = 1;
    }
    else
    {
        mMeasStep = 0;
        ADC1->CR2 |= ADC_CR2_SWSTART;
    }
}

void SmHwBattery::onSleep(void)
{
    ADC1->CR2 &= ~ADC_CR2_ADON;
    mGpioEn->resetPin();
}

void SmHwBattery::onWake(void)
{
    ADC1->CR2 |= ADC_CR2_ADON;
    mMeasStep = 0;
    onTimer(0);
}

extern "C"
{

void ADC1_2_IRQHandler (void)
{
    SmHwBattery::getInstance()->mValue = ADC1->DR;
    SmHwBattery::getInstance()->mGpioEn->resetPin();
    SmHalSysTimer::subscribe(SmHwBattery::getInstance(), MEAS_INTERVAL, false);
}

}
