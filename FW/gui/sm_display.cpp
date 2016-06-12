#include "sm_display.h"
#include "smartcanvas.h"
#include "smartfont.h"
#include "smartstorage.h"
#include "smartanimator.h"
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

/// Set COM Output Scan Direction from COM0 to COM[N –1]
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
    :texture(0)
{
}

void SmDisplay::init(int width, int height, SmHalAbstractSpi * spi, SmHalAbstractGpio * dc, SmHalAbstractGpio * power)
{
//    LcdInit();
    if (texture)
        delete texture;

    texture = new SmTexture();
    texture->init(width, height);
    mSpi = spi;
    mDcPin = dc;
    mPowerPin = power;
    mDcPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_50M);
    mPowerPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_50M);

    mPowerPin->resetPin();
    mDcPin->resetPin();

//    spi_Init();

    sendCommand(LCD_CMD_DISPLAY_OFF);//+ display off

    sendCommand(LCD_CMD_SET_COL_L);
    sendCommand(LCD_CMD_SET_COL_H);
    sendCommand(LCD_CMD_START_LINE);//+ Set Page Start Address for Page Addressing Mode,0-7

    sendCommand(LCD_CMD_CONTRAST, 0xFF); ///~~~0xCF);

    sendCommand(LCD_CMD_SEGMENT_MAP_1);
    sendCommand(LCD_CMD_SCAN_DIR_MINUS);
    sendCommand(LCD_CMD_NORMAL_DISPLAY);

//--set multiplex ratio(1 to 64)
    sendCommand(LCD_CMD_MULTIPLEX_RATIO, 63);
//-set display offset    (not offset)
    sendCommand(LCD_CMD_DISPLAY_OFFSET, 0x00);

//--set display clock divide ratio/oscillator frequency
//   set divide ratio
    sendCommand(LCD_CMD_SET_CLOCK_DIVIDE, 0xF0);//

    sendCommand(LCD_CMD_SET_PRE_CHARGE_PERIOD, 0xF1);

    sendCommand(LCD_CMD_SET_PIN_CONF, 0x12);//--  0x02???

    sendCommand(LCD_CMD_SET_VCOMH, 0x40);//--
//    LCDCommand_param(LCD_CMD_SET_VCOMH, 0x20);  // 0.77 * VCC
//    LCDCommand_param(LCD_CMD_SET_VCOMH, 0x10);  // 0.77 * VCC

// Set Memory Addressing Mode
    sendCommand(LCD_CMD_ADDR_MODE, LCD_CMD_ADDR_MODE_PAGE);

    sendCommand(LCD_CMD_SET_CHARGE_BUMP, 0x14); // Enable Charge Pump

    sendCommand(LCD_CMD_ALL_OFF);
    sendCommand(LCD_CMD_NORMAL_DISPLAY);

    fill(0x00);

    sendCommand(LCD_CMD_DISPLAY_ON);//+ --turn on oled panel
}

void SmDisplay::setPix(int x, int y, int value)
{
    for (int i = 0; i < 8; i++)
        for(int j = 0; j < 128; j++)
        {
            if (j%2)
                texture->getPData()[i*128+j] = 0x55;
            else
                texture->getPData()[i*128+j] = 0xAA;
        }
}

void SmDisplay::update(void)
{
    uint8_t m = 0;
    uint8_t n = 0;

    uint8_t *pData = texture->getPData();

    for(m=0; m<8; m++)
    {
        sendCommand(LCD_CMD_SET_PAGE + m);    // page0-page1
        sendCommand(LCD_CMD_SET_COL_L);         // low column start address
        sendCommand(LCD_CMD_SET_COL_H);        // high column start address

        for(n=0; n<128; n++)
        {
            sendData(pData++,1);
        }
    }
}

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
    mSpi->setSs();
    mSpi->resetSs();
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

/// @todo Only for testing
void SmDisplay::fill(uint8_t data)
{
    char m = 0;
    char n = 0;
    for(m=0; m<8; m++)
    {
        sendCommand(LCD_CMD_SET_PAGE + m);    // page0-page1
        sendCommand(LCD_CMD_SET_COL_L);         // low column start address
        sendCommand(LCD_CMD_SET_COL_H);        // high column start address

        for(n=0; n<128; n++)
        {
            sendData(&data,1);
        }
    }
}

void SmDisplay::powerOn(void)
{
    mPowerPin->resetPin();
}

void SmDisplay::powerOff(void)
{
    mPowerPin->setPin();
}
