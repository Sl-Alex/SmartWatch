#include "sm_desktop.h"
#include "sm_image.h"

/// @todo Implement all services initialization
void SmDesktop::init(SmCanvas * canvas)
{
    SmHwBattery::getInstance()->init();
    SmHalSysTimer::subscribe(this, 10, true);

    pCanvas = canvas;
    mBatteryLevel = 100;
    mChargeStatus= false;
    for (uint8_t i = 0; i < MAX_ICONS_COUNT; ++i)
    {
        setIcon(i,INVALID_ICON);
    }
}

void SmDesktop::onTimer(uint32_t timeStamp)
{
    uint8_t newBatteryLevel = SmHwBattery::getInstance()->getCharge();
    bool newChargeStatus = SmHwBattery::getInstance()->getStatus();
    if ((newBatteryLevel != mBatteryLevel) ||
        (newChargeStatus != mChargeStatus))
    {
        mBatteryLevel = newBatteryLevel;
        mChargeStatus = newChargeStatus;

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
        setIcon(ICON_POS_BATT, iconNum);

        if (mChargeStatus)
        {
            setIcon(ICON_POS_POWER, 12);
        }
        else
        {
            setIcon(ICON_POS_POWER, INVALID_ICON);
        }
        drawIcons();
    }
}

void SmDesktop::setIcon(uint8_t pos, uint8_t icon)
{
    icons[pos] = icon;
}

void SmDesktop::drawIcons(void)
{
    SmImage image;
    uint8_t x_off = 128;

    pCanvas->fillRect(64,0,127,7,0);

    for (int8_t i = MAX_ICONS_COUNT - 1; i >= 0; --i)
    {
        if (icons[i] == INVALID_ICON)
            continue;

        image.init(icons[i]);
        x_off -= image.getWidth();
        pCanvas->drawCanvas(x_off,0,&image);
        x_off--;
    }
}
