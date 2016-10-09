#include "sm_desktop.h"
#include "sm_image.h"
#include "sm_hal_rtc.h"
#include "sm_hw_storage.h"
#include <cstdio>

/// @todo Implement all services initialization
void SmDesktop::init(SmCanvas * canvas)
{
    SmHwBattery::getInstance()->init();
    SmHalSysTimer::subscribe(this, 10, true);

    pCanvas = canvas;
    mBatteryLevel = 50;
    mBatteryStatus = SmHwBattery::BATT_STATUS_DISCHARGING;
    for (uint8_t i = 0; i < MAX_ICONS_COUNT; ++i)
    {
        setIcon(i,INVALID_ICON);
    }

    pFont7SegBig = new SmFont();
    pFont7SegBig->init(17);
    pFont7SegSmall = new SmFont();
    pFont7SegSmall->init(18);
    pFontSmall = new SmFont();
    pFontSmall->init(IDX_FW_FONT_SMALL);
}

SmDesktop::~SmDesktop()
{
    delete pFont7SegBig;
    delete pFont7SegSmall;
    delete pFontSmall;
}

void SmDesktop::onTimer(uint32_t timeStamp)
{
    uint8_t newBatteryLevel = SmHwBattery::getInstance()->getCharge();
    SmHwBattery::BatteryStatus newChargeStatus = SmHwBattery::getInstance()->getStatus();
    if ((newBatteryLevel != mBatteryLevel) ||
        (newChargeStatus != mBatteryStatus))
    {
        mBatteryLevel = newBatteryLevel;
        mBatteryStatus = newChargeStatus;

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

        if (mBatteryStatus != SmHwBattery::BATT_STATUS_DISCHARGING)
        {
            setIcon(ICON_POS_POWER, 12);
        }
        else
        {
            setIcon(ICON_POS_POWER, INVALID_ICON);
        }
        if (mBatteryStatus == SmHwBattery::BATT_STATUS_CHARGED)
        {
            setIcon(ICON_POS_BATT-1, 12);
        }
        else
        {
            setIcon(ICON_POS_BATT-1, INVALID_ICON);
        }
        drawIcons();
    }
    uint16_t txt[] = {0x12, 0x13, 0x1A, 0x14, 0x15};
    uint16_t txt2[] = {0x15,0x19};
#define OFFS 0
#define VSZ1 41
#define W1   21
#define VSZ2 21
#define W2   11
    SmHalRtc::SmHalRtcTime rtc = SmHalRtc::getInstance()->getDateTime();

    txt[0] = 0x10 + rtc.hour / 10;
    txt[1] = 0x10 + rtc.hour % 10;

    txt[3] = 0x10 + rtc.minute / 10;
    txt[4] = 0x10 + rtc.minute % 10;

    txt2[0] = 0x10 + rtc.second / 10;
    txt2[1] = 0x10 + rtc.second % 10;

    char date[11];
    sprintf(date,"%2u/%02u/%04u",rtc.day,rtc.month,rtc.year);
    pCanvas->fillRect(0, 0, 80, 8, 0);
    pFontSmall->drawText(pCanvas,0,0,date);

    pFont7SegBig->drawText(pCanvas,0,64-OFFS-VSZ1,&txt[0],1);
    pFont7SegBig->drawText(pCanvas,21+3,64-OFFS-VSZ1,&txt[1],1);
    pFont7SegBig->drawText(pCanvas,21+3+21+3,64-OFFS-VSZ1,&txt[2],1);
    pFont7SegBig->drawText(pCanvas,21+3+21+3+3+3,64-OFFS-VSZ1,&txt[3],1);
    pFont7SegBig->drawText(pCanvas,21+3+21+3+3+3+21+3,64-OFFS-VSZ1,&txt[4],1);
    pFont7SegSmall->drawText(pCanvas,21+3+21+3+3+3+21+3+21+3,64-OFFS-VSZ2,&txt2[0],1);
    pFont7SegSmall->drawText(pCanvas,21+3+21+3+3+3+21+3+21+3+11+3,64-OFFS-VSZ2,&txt2[1],1);
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
