/**
    @file sm_hw_battery.cpp
*/
#include "sm_hw_battery.h"

#ifndef PC_SOFTWARE
#include "stm32f10x.h"
#include "sm_hal_gpio.h"

#define GPIO_EN_PORT    GPIOA_BASE  ///< Battery measurement enable port
#define GPIO_EN_PIN     2           ///< Battery measurement enable pin
#define MEAS_DELAY      2           ///< Delay between enabling and measurement
#define MEAS_INTERVAL   500         ///< Battery voltage update interval

#define GPIO_AIN_PORT   GPIOA_BASE  ///< Battery voltage input port
#define GPIO_AIN_PIN    0           ///< Battery voltage input pin
#endif

#define BAT_MAX 2544    ///< Maximum raw ADC battery voltage
#define BAT_MIN 2048    ///< Minimum raw ADC battery voltage

#define VOLT_MAX 4100   ///< Maximum battery voltage
#define VOLT_MIN 3300   ///< Minimum battery voltage


void SmHwBattery::init()
{
    mRaw = 0;

#ifndef PC_SOFTWARE
    mGpioEn = new SmHalGpio<GPIO_EN_PORT, GPIO_EN_PIN>();
    mGpioEn->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);
    mGpioEn->setPin();

    SmHalGpio<GPIO_AIN_PORT, GPIO_AIN_PIN> BatGpio;
    BatGpio.setModeSpeed(SM_HAL_GPIO_MODE_AIN, SM_HAL_GPIO_SPEED_2M);

    ADC1->SQR3 = 0;

    ADC1->CR2 = ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG;

    ADC1->CR2 |= ADC_CR2_ADON;

    // Wait a bit before ADC calibration
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

    SmHwPowerMgr::getInstance()->subscribe(this);

    mMeasStep = 0;
#endif
    SmHalSysTimer::subscribe(this,1,false);
}

void SmHwBattery::onTimer(uint32_t timeStamp)
{
#ifndef PC_SOFTWARE
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
#else
    mRaw = 55;
    updateValues();
#endif
}

#ifndef PC_SOFTWARE
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

/// @brief ADC interrupt handler.
/// Measurement enable circuit is disabled after the measurement.
void ADC1_2_IRQHandler (void)
{
    SmHwBattery::getInstance()->mRaw = ADC1->DR;
    SmHwBattery::getInstance()->mGpioEn->resetPin();
    SmHalSysTimer::subscribe(SmHwBattery::getInstance(), MEAS_INTERVAL, false);
    SmHwBattery::getInstance()->updateValues();
}

}
#endif

void SmHwBattery::updateValues(void)
{
    if (mRaw > BAT_MAX) mRaw = BAT_MAX;
	if (mRaw < BAT_MIN) mRaw = BAT_MIN;

    mCharge = (mRaw - BAT_MIN)*100/(BAT_MAX - BAT_MIN);
	mVoltage = VOLT_MIN + (mRaw - BAT_MIN)*(VOLT_MAX - VOLT_MIN)/(BAT_MAX - BAT_MIN);
}
