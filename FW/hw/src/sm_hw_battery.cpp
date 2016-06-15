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

    // Subscribe for the further update notifications
    SmHalSysTimer::subscribe(this, 10000, false);
}

uint32_t SmHwBattery::readValue(void)
{
    uint32_t val = 0;
    // Start measurement
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) != ADC_SR_EOC)
    {

    }
    val = ADC1->DR;
    return val;
}

void SmHwBattery::onTimer(uint32_t timeStamp)
{
    static uint8_t step = 0;

    if (step == 0)
    {
        mGpioEn->setPin();
        SmHalSysTimer::subscribe(this, MEAS_DELAY, false);
        step = 1;
    }
    else
    {
        step = 0;
        ADC1->CR2 |= ADC_CR2_SWSTART;
    }
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
