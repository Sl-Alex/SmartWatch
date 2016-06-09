#include "sm_display.h"
#include "smartcanvas.h"
#include "smartfont.h"
#include "smartstorage.h"
#include "smartanimator.h"
#include "sh1106.h"
#include "stdio.h"

SmDisplay::SmDisplay()
{
}

void SmDisplay::init(int width, int height, SmHwAbstractSpi * spi, SmHwAbstractGpio * dc, SmHwAbstractGpio * power)
{
//    LcdInit();
    mSpi = spi;
    mDcPin = dc;
    mPowerPin = power;

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
    sendCommand(LCD_CMD_ADDR_MODE, 0x02);

    sendCommand(LCD_CMD_SET_CHARGE_BUMP, 0x14); // Enable Charge Pump

    sendCommand(LCD_CMD_ALL_OFF);
    sendCommand(LCD_CMD_NORMAL_DISPLAY);

    sendCommand(LCD_CMD_DISPLAY_ON);//+ --turn on oled panel

//    LcdClear();
}

void SmDisplay::setPix(int x, int y, int value)
{
    texture->getPData()[0] = 0x55;
}

void SmDisplay::update(void)
{
//    pAnimator->tick();
//    if (!pAnimator2->tick())
//    {
//        imageIndex++;
//        if (imageIndex >= IMG_CNT) imageIndex = 0;
//        pImage->init(IMG_OFFSET + imageIndex);
//        pAnimator2->start(125, 16, 0, 0, 32, 32);
//    }
    LcdOut(texture->getPData());
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
