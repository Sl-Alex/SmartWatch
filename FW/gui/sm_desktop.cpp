#include "sm_desktop.h"
#include "sm_image.h"

/// @todo Implement all services initialization
void SmDesktop::init(SmCanvas * canvas)
{
    SmHwBattery::getInstance()->init();
    SmHalSysTimer::subscribe(this, 10, true);

    pCanvas = canvas;
}

void SmDesktop::onTimer(uint32_t timeStamp)
{
    uint8_t newBatteryLevel = SmHwBattery::getInstance()->getCharge();
    if (newBatteryLevel != mBatteryLevel)
    {
        mBatteryLevel = newBatteryLevel;
        SmImage * pBatImage = new SmImage();
        uint8_t iconNum = 5; // Base number
        if (mBatteryLevel <= 5)
        {
            // Do nothing
        }
        else if (mBatteryLevel <= 15)
        {
            iconNum += 1;
        }
        else if (mBatteryLevel <= 30)
        {
            iconNum += 2;
        }
        else if (mBatteryLevel <= 45)
        {
            iconNum += 3;
        }
        else if (mBatteryLevel <= 65)
        {
            iconNum += 4;
        }
        else if (mBatteryLevel <= 85)
        {
            iconNum += 5;
        }
        else
        {
            iconNum += 6;
        }

        pBatImage->init(iconNum);
        pCanvas->drawCanvas(123, 0, pBatImage);
    }
    mBatteryLevel = SmHwBattery::getInstance()->getCharge();
}
