#include "sm_display.h"

#ifndef PC_SOFTWARE
#include "sm_hal_spi_sw.h"
#include "sm_hal_gpio.h"
#endif

#include "sm_canvas.h"
#include "sm_font.h"
#include "sm_animator.h"
#include "stdio.h"

/// Set Lower Column Start Address for Page Addressing Mode (00h~0Fh)
#define LCD_CMD_SET_COL_L               0x00

/// Set Higher Column Start Address for Page Addressing Mode (10h~1Fh)
#define LCD_CMD_SET_COL_H               0x10

/// @brief Set Memory Addressing Mode (20h)
/// @details Next byte:
///  0 - Horizontal Addressing Mode;
///  1 - Vertical Addressing Mode;
///  2 - Page Addressing Mode (RESET);
///  3 - Invalid
#define LCD_CMD_ADDR_MODE               0x20
#define LCD_CMD_ADDR_MODE_HOR           0
#define LCD_CMD_ADDR_MODE_VERT          1
#define LCD_CMD_ADDR_MODE_PAGE          2       ///< Default

/// Set Display Start Line (40h~7Fh)
#define LCD_CMD_START_LINE              0x40

/// Set Contrast Control for BANK0 (81h)
#define LCD_CMD_CONTRAST                0x81

/// Charge Bump Setting
#define LCD_CMD_SET_CHARGE_BUMP         0x8D

/// Column address 0 is mapped to SEG0 (RESET)
#define LCD_CMD_SEGMENT_MAP_0           0xA0
/// column address 127 is mapped to SEG0
#define LCD_CMD_SEGMENT_MAP_1           0xA1

/// Entire Display OFF
#define LCD_CMD_ALL_OFF                 0xA4
/// Entire Display ON
#define LCD_CMD_ALL_ON                  0xA5

/// Set normal display
#define LCD_CMD_NORMAL_DISPLAY          0xA6
/// Set inverse display
#define LCD_CMD_INVERSE_DISPLAY         0xA7

/// Set Multiplex Ratio
#define LCD_CMD_MULTIPLEX_RATIO         0xA8

/// Enable built-in DC/DC
#define LCD_CMD_DCDC_ON                 0xAD

/// Set Display ON
#define LCD_CMD_DISPLAY_ON              0xAF
/// Set Display OFF
#define LCD_CMD_DISPLAY_OFF             0xAE

/// Set Page Start Address for Page Addressing Mode
#define LCD_CMD_SET_PAGE                0xB0

/// Set COM Output Scan Direction from COM0 to COM[N-1]
#define LCD_CMD_SCAN_DIR_PLUS           0xC0
/// Set COM Output Scan Direction from COM[N-1] to COM0
#define LCD_CMD_SCAN_DIR_MINUS          0xC8

/// Set Display Offset
#define LCD_CMD_DISPLAY_OFFSET          0xD3

/// Set Display Clock Divide Ratio/ Oscillator Frequency
#define LCD_CMD_SET_CLOCK_DIVIDE        0xD5

/// Set Pre-charge Period
#define LCD_CMD_SET_PRE_CHARGE_PERIOD   0xD9

/// Set COM Pins Hardware Configuration
#define LCD_CMD_SET_PIN_CONF            0xDA

/// Set VCOMH Deselect Level
#define LCD_CMD_SET_VCOMH               0xDB

SmDisplay::SmDisplay()
    :mCanvas(0)
{
#ifndef PC_SOFTWARE
    using DisplaySpi = SmHalSpiSw<SM_HAL_SPI_MODE0, SM_HAL_SPI_CFG_OUT, SM_HAL_SPI_WIDTH_8>;
    mSpi = new DisplaySpi();
    mSpi->setSsPin(new SmHalGpio<GPIOB_BASE,5>());
    ((DisplaySpi *)mSpi)->init(new SmHalGpio<GPIOB_BASE,8>(),   // SCK
               0,                                               // MISO - not used in SM_HAL_SPI_CFG_OUT configuration
               new SmHalGpio<GPIOB_BASE,9>());                  // MOSI

    mDcPin = new SmHalGpio<GPIOB_BASE,7>();
    mPowerPin = new SmHalGpio<GPIOC_BASE,13>();
    mResetPin = new SmHalGpio<GPIOB_BASE, 6>();

    mDcPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_50M);
    mPowerPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_50M);
    mResetPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);

    mDcPin->resetPin();
    mResetPin->setPin();
    mPowered = false;

    SmHwPowerMgr::getInstance()->subscribe(this);
#endif
}

void SmDisplay::init(int width, int height)
{
    if (mCanvas)
        delete mCanvas;

    mCanvas = new SmCanvas();
    mCanvas->init(width, height);
}

void SmDisplay::update(void)
{
    if (!mPowered)
        return;
#ifndef PC_SOFTWARE
    uint8_t m = 0;

    uint8_t *pData = mCanvas->getPData();

    for(m=0; m<8; m++)
    {
        sendCommand(LCD_CMD_SET_PAGE + m);    // page0-page1
        sendCommand(LCD_CMD_SET_COL_L);         // low column start address
        sendCommand(LCD_CMD_SET_COL_H);        // high column start address
        sendData(pData,128);
        pData += 128;
    }
#endif
}

#ifndef PC_SOFTWARE
void SmDisplay::sendCommand(uint8_t cmd)
{
    mDcPin->resetPin();
    mSpi->resetSs();
    mSpi->transfer(0, &cmd, 1);
    mSpi->setSs();
}

void SmDisplay::sendCommand(uint8_t cmd, uint8_t data)
{
    mDcPin->resetPin();
    mSpi->resetSs();
    mSpi->transfer(0, &cmd, 1);
    mSpi->transfer(0, &data, 1);
    mSpi->setSs();
}

void SmDisplay::sendData(uint8_t * data, uint8_t size)
{
    mDcPin->setPin();
    mSpi->resetSs();
    mSpi->transfer(0, data, size);
    mSpi->setSs();
}
#endif

void SmDisplay::powerOn(void)
{
    if (mPowered)
        return;

    mPowered = true;
#ifndef PC_SOFTWARE
    mPowerPin->resetPin();
    mResetPin->setPin();
    mSpi->init();

    sendCommand(LCD_CMD_DISPLAY_OFF);

    sendCommand(LCD_CMD_SET_COL_L);
    sendCommand(LCD_CMD_SET_COL_H);
    sendCommand(LCD_CMD_START_LINE);

    sendCommand(LCD_CMD_CONTRAST, 0xFF);

    sendCommand(LCD_CMD_SEGMENT_MAP_1);
    sendCommand(LCD_CMD_SCAN_DIR_MINUS);
    sendCommand(LCD_CMD_NORMAL_DISPLAY);

    // Set multiplex ratio(1 to 64)
    sendCommand(LCD_CMD_MULTIPLEX_RATIO, 63);
    // Set display offset
    sendCommand(LCD_CMD_DISPLAY_OFFSET, 0x00);

    // Set display clock divide ratio/oscillator frequency
    sendCommand(LCD_CMD_SET_CLOCK_DIVIDE, 0xF0);//

    sendCommand(LCD_CMD_SET_PRE_CHARGE_PERIOD, 0xF1);

    sendCommand(LCD_CMD_SET_PIN_CONF, 0x12);

    sendCommand(LCD_CMD_SET_VCOMH, 0x40);

    // Set Memory Addressing Mode
    sendCommand(LCD_CMD_ADDR_MODE, LCD_CMD_ADDR_MODE_PAGE);
    // Enable Charge Pump
    sendCommand(LCD_CMD_SET_CHARGE_BUMP, 0x14);

    sendCommand(LCD_CMD_ALL_OFF);
    sendCommand(LCD_CMD_NORMAL_DISPLAY);

    update();

    sendCommand(LCD_CMD_DISPLAY_ON);
#endif
}

void SmDisplay::powerOff(void)
{
    mPowered = false;
#ifndef PC_SOFTWARE
    mSpi->deInit();
    mDcPin->resetPin();
    mResetPin->resetPin();
    mPowerPin->setPin();
#endif
}

void SmDisplay::onSleep(void)
{
#ifndef PC_SOFTWARE
    powerOff();
#endif
}

void SmDisplay::onWake(uint32_t wakeSource)
{
#ifndef PC_SOFTWARE
    if (wakeSource & (SmHwPowerMgr::WakeSource::SM_HW_WAKE_MASK_ACCELEROMETER |
                      SmHwPowerMgr::WakeSource::SM_HW_WAKE_MASK_KEYBOARD))
    {
        SmHwPowerMgr::getInstance()->allowSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_DISPLAY, 5000);
        powerOn();
    }
#endif
}
