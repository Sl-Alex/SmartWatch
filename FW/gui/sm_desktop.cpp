#include "sm_desktop.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_display.h"
#include "sm_hal_rtc.h"
#include "sm_hw_storage.h"
#include "sm_hw_keyboard.h"
#include <cstdio>

/// @todo Implement all services initialization
void SmDesktop::init(void)
{
    SmHwBattery::getInstance()->init();
    SmHalSysTimer::subscribe(this, 10, true);
    SmHwKeyboard::getInstance()->initSubscribersPool(10);
    SmHwKeyboard::getInstance()->subscribe(this);

    pCanvas = SmDisplay::getInstance()->getCanvas();
    mBatteryLevel = 50;
    mBatteryStatus = SmHwBattery::BATT_STATUS_DISCHARGING;
    for (uint8_t i = 0; i < MAX_ICONS_COUNT; ++i)
    {
        setIcon(i,INVALID_ICON);
    }

    pFont7SegBig = new SmFont();
    pFont7SegBig->init(IDX_FW_FONT_CLOCK_BIG);
    pFont7SegSmall = new SmFont();
    pFont7SegSmall->init(IDX_FW_FONT_CLOCK_SMALL);
    pFontSmall = new SmFont();
    pFontSmall->init(IDX_FW_FONT_SMALL);
    pMainMenu = nullptr;
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->blockSleep();
    SmHwPowerMgr::getInstance()->allowSleep(5000);
#endif
}

SmDesktop::~SmDesktop()
{
    delete pFont7SegBig;
    delete pFont7SegSmall;
    delete pFontSmall;
}

void SmDesktop::onTimer(uint32_t)
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.tick();
        if (menuAnimator.isRunning() == false)
        {
            // Delete temporary and restore original canvas
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
            // Clear battery level (icons will be updated)
            mBatteryLevel = 0;
        }
    }

    uint8_t newBatteryLevel = SmHwBattery::getInstance()->getCharge();
    SmHwBattery::BatteryStatus newChargeStatus = SmHwBattery::getInstance()->getStatus();
    if ((newBatteryLevel != mBatteryLevel) ||
        (newChargeStatus != mBatteryStatus))
    {
        mBatteryLevel = newBatteryLevel;
        mBatteryStatus = newChargeStatus;

        uint8_t iconNum = 7; // Base number
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
            setIcon(ICON_POS_POWER, 14);
        }
        else
        {
            setIcon(ICON_POS_POWER, INVALID_ICON);
        }
        if (mBatteryStatus == SmHwBattery::BATT_STATUS_CHARGED)
        {
            setIcon(ICON_POS_BATT-1, 14);
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
    if (pMainMenu != nullptr)
        return;

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

void SmDesktop::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_SELECT)
    {
        if (pMainMenu == nullptr)
        {
            // Check if pCanvas is different, delete and set default then
            menuAnimator.finish();
            if (pCanvas != SmDisplay::getInstance()->getCanvas())
            {
                delete pCanvas;
                pCanvas = SmDisplay::getInstance()->getCanvas();
            }
            SmHwKeyboard::getInstance()->unsubscribe(this);
            SmHalSysTimer::unsubscribe(this);
            pMainMenu = new SmMainMenu(this);
        }
    }
    if (key == SM_HW_BUTTON_VIRT_EXIT)
    {
        if (pMainMenu != nullptr)
        {
            delete pMainMenu;
            pMainMenu = nullptr;
            // Replace drawing canvas with this
            pCanvas = new SmCanvas();
            pCanvas->init(128,64);
            pCanvas->clear();

            menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(), pCanvas);
            menuAnimator.setDirection(SmAnimator::ANIM_DIR_DOWN);
            menuAnimator.setShiftLimit(128);
            menuAnimator.setSpeed(2);
            menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
            menuAnimator.start(0,0,0,0,128,64);

            SmHwKeyboard::getInstance()->subscribe(this);
            SmHalSysTimer::subscribe(this,10,true);
        }
    }
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->blockSleep();
#endif
}

void SmDesktop::onKeyUp(SmHwButtons)
{
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->allowSleep(2000);
#endif
}
