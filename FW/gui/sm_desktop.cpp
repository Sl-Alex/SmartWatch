#include "sm_desktop.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_display.h"
#include "sm_hal_rtc.h"
#include "sm_hw_storage.h"
#include "sm_hw_keyboard.h"
#include <cstdio>

const char FwVersion[10] = "FW v0.4";

/// @todo Implement all services initialization
void SmDesktop::init(void)
{
    SmHalRtc::getInstance()->init();
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
    pFont7SegBig->init(IDX_FW_FONT_1_LARGE);
    pFont7SegSmall = new SmFont();
    pFont7SegSmall->init(IDX_FW_FONT_1_MEDIUM);
    pFontSmall = new SmFont();
    pFontSmall->init(IDX_FW_FONT_SMALL);
    pMainMenu = nullptr;
    pNotification = nullptr;
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->allowSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_DISPLAY,
                                            SmHwPowerMgr::SleepTimeout::SM_HW_SLEEP_LONG);
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
        if (!menuAnimator.isRunning())
        {
            // Delete temporary and restore original canvas
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
            // Clear battery level (icons will be updated)
            mBatteryLevel = 0;
        }
        return;
    }

    drawAll();
}

void SmDesktop::drawAll(void)
{
    mBatteryLevel = SmHwBattery::getInstance()->getCharge();
    mBatteryStatus = SmHwBattery::getInstance()->getStatus();

    uint8_t iconNum = 12; // Base number
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
        setIcon(ICON_POS_POWER, 19);
    }
    else
    {
        setIcon(ICON_POS_POWER, INVALID_ICON);
    }
    if (mBatteryStatus == SmHwBattery::BATT_STATUS_CHARGED)
    {
        setIcon(ICON_POS_BATT-1, 19);
    }
    else
    {
        setIcon(ICON_POS_BATT-1, INVALID_ICON);
    }
    drawIcons();

    uint16_t txt[] = {0x12, 0x13, 0x1A, 0x14, 0x15};
    uint16_t txt2[] = {0x15,0x19};
    uint32_t V1 = 64 - pFont7SegBig->getFontHeight();
#define W1   21
    uint32_t V2 = 64 - pFont7SegSmall->getFontHeight();
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

    pFont7SegBig->drawText(pCanvas,0,V1,&txt[0],1);
    pFont7SegBig->drawText(pCanvas,21+3,V1,&txt[1],1);
    pFont7SegBig->drawText(pCanvas,21+3+21+3,V1,&txt[2],1);
    pFont7SegBig->drawText(pCanvas,21+3+21+3+3+3,V1,&txt[3],1);
    pFont7SegBig->drawText(pCanvas,21+3+21+3+3+3+21+3,V1,&txt[4],1);
    pFont7SegSmall->drawText(pCanvas,21+3+21+3+3+3+21+3+21+3,V2,&txt2[0],1);
    pFont7SegSmall->drawText(pCanvas,21+3+21+3+3+3+21+3+21+3+11+3,V2,&txt2[1],1);
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
            if (menuAnimator.isRunning())
            {
                menuAnimator.finish();
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

            drawAll();

            menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(), pCanvas);
            menuAnimator.setDirection(SmAnimator::ANIM_DIR_DOWN);
            menuAnimator.setShiftLimit(128);
            menuAnimator.setSpeed(4);
            menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
            menuAnimator.start(0,0,0,0,128,64);

            SmHwKeyboard::getInstance()->subscribe(this);
            SmHalSysTimer::subscribe(this,10,true);
        }
        if (pNotification != nullptr)
        {
            delete pNotification;
            pNotification = nullptr;

            // Replace drawing canvas with this
            pCanvas = new SmCanvas();
            pCanvas->init(128,64);
            pCanvas->clear();

            drawAll();

            menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(), pCanvas);
            menuAnimator.setDirection(SmAnimator::ANIM_DIR_DOWN);
            menuAnimator.setShiftLimit(128);
            menuAnimator.setSpeed(4);
            menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
            menuAnimator.start(0,0,0,0,128,64);

            SmHwKeyboard::getInstance()->subscribe(this);
            SmHalSysTimer::subscribe(this,10,true);
        }
    }
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->allowSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_DISPLAY,
                                            SmHwPowerMgr::SleepTimeout::SM_HW_SLEEP_LONG);
#endif
}

void SmDesktop::onKeyUp(SmHwButtons)
{
}

void SmDesktop::showNotification(void)
{
    if (pNotification == nullptr)
    {
        if (menuAnimator.isRunning())
        {
            menuAnimator.finish();
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
        }
        // Check if pCanvas is different, delete and set default then
        SmHwKeyboard::getInstance()->unsubscribe(this);
        SmHalSysTimer::unsubscribe(this);
        pNotification = new SmNotification(this);
    }
}

const char * SmDesktop::getVersion(void)
{
    return FwVersion;
}
