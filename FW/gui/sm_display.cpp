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

void SmDisplay::init(int width, int height)
{
    LcdInit();
}

void SmDisplay::setInterface(SmHwAbstractSpi * spi, SmHwGpioPin * dc)
{
    mSpi = spi;
    mDcPin = dc;
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
    mSpi->transfer(0, &data, 1);
    mSpi->setSs();
}
