#include "sm_desktop.h"
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_display.h"
#include "sm_hal_rtc.h"
#include "sm_hw_storage.h"
#include "sm_hw_keyboard.h"
#include <cstdio>

const char FwVersion[10] = "FW v0.4";

#define DIGITS_ANIMATION_SPEED  2
#define DIGITS_SPACE            3

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

    uint8_t bigDigitHeight = pFont7SegBig->getFontHeight();
    uint8_t bigDigitWidth = pFont7SegBig->getSymbolWidth(0x30 - 0x20);
    uint8_t smallDigitHeight = pFont7SegSmall->getFontHeight();
    uint8_t smallDigitWidth = pFont7SegSmall->getSymbolWidth(0x30 - 0x20);

    // Hours
    mDigitsOffsetX[0] = 0;
    mDigitsOffsetX[1] = mDigitsOffsetX[0] + bigDigitWidth + DIGITS_SPACE;
    // Minutes
    mDigitsOffsetX[2] = mDigitsOffsetX[1] + bigDigitWidth + 2*DIGITS_SPACE + pFont7SegBig->getSymbolWidth(':' - 0x20);
    mDigitsOffsetX[3] = mDigitsOffsetX[2] + bigDigitWidth + DIGITS_SPACE;
    // Seconds
    mDigitsOffsetX[4] = mDigitsOffsetX[3] + bigDigitWidth + DIGITS_SPACE;
    mDigitsOffsetX[5] = mDigitsOffsetX[4] + smallDigitWidth + DIGITS_SPACE;

    uint32_t Y1 = pCanvas->getHeight() - bigDigitHeight;
    uint32_t Y2 = pCanvas->getHeight() - smallDigitHeight;
    // Hours
    mDigitsOffsetY[0] = Y1;
    mDigitsOffsetY[1] = Y1;
    // Minutes
    mDigitsOffsetY[2] = Y1;
    mDigitsOffsetY[3] = Y1;
    // Seconds
    mDigitsOffsetY[4] = Y2;
    mDigitsOffsetY[5] = Y2;

    for (uint8_t i = 0; i < DIGITS_COUNT; ++i)
    {
        if (i < 4)
        {
            mNewDigits[i].init(bigDigitWidth, bigDigitHeight);
            pFont7SegBig->drawSymbol(&mNewDigits[i],0,0, 0x10);
        }
        else
        {
            mNewDigits[i].init(smallDigitWidth, smallDigitHeight);
            pFont7SegSmall->drawSymbol(&mNewDigits[i],0,0, 0x10);
        }

        mDigitAnimators[i].setDirection(SmAnimator::ANIM_DIR_DOWN);
        mDigitAnimators[i].setType(SmAnimator::ANIM_TYPE_SHIFT);
        mDigitAnimators[i].setDestSource(pCanvas, &mNewDigits[i]);
        mDigitAnimators[i].setShiftLimit(mNewDigits[i].getHeight());
        mDigitAnimators[i].setSpeed(DIGITS_ANIMATION_SPEED);
        mDigitAnimators[i].start(
                    mDigitsOffsetX[i],
                    mDigitsOffsetY[i],
                    0,0,
                    mNewDigits[i].getWidth(),
                    mNewDigits[i].getHeight());
    }
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->subscribe(this);
#endif
}

SmDesktop::~SmDesktop()
{
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->unsubscribe(this);
#endif
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
    else
    {
        for (uint8_t i = 0; i < DIGITS_COUNT; ++i)
        {
            if (mDigitAnimators[i].isRunning())
                mDigitAnimators[i].tick();
        }
    }

    drawAll();
}

void SmDesktop::onSleep(void)
{

}

void SmDesktop::onWake(uint32_t WakeSource)
{
    if (pNotification == nullptr)
    {
        drawTime();
    }
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

    uint16_t separator = ':' - 0x20;
    SmHalRtc::SmHalRtcTime rtc = SmHalRtc::getInstance()->getDateTime();

    static uint8_t newDigits[DIGITS_COUNT];

    newDigits[0] = rtc.hour / 10;
    newDigits[1] = rtc.hour % 10;
    newDigits[2] = rtc.minute / 10;
    newDigits[3] = rtc.minute % 10;
    newDigits[4] = rtc.second / 10;
    newDigits[5] = rtc.second % 10;

    for (uint8_t i = 0; i < DIGITS_COUNT; ++i)
    {
        if (newDigits[i] != mDigits[i])
        {
            mDigitAnimators[i].finish();

            if (i < 4)
                pFont7SegBig->drawSymbol(&mNewDigits[i],0,0, 0x10+newDigits[i]);
            else
                pFont7SegSmall->drawSymbol(&mNewDigits[i],0,0, 0x10+newDigits[i]);

            mDigitAnimators[i].setShiftLimit(mNewDigits[i].getHeight());
            mDigitAnimators[i].setSpeed(DIGITS_ANIMATION_SPEED);
            mDigitAnimators[i].start(mDigitsOffsetX[i],mDigitsOffsetY[i],0,0,mNewDigits[i].getWidth(),mNewDigits[i].getHeight());

            mDigits[i] = newDigits[i];
        }
    }
    pFont7SegBig->drawText(pCanvas,mDigitsOffsetX[2] - DIGITS_SPACE - pFont7SegBig->getSymbolWidth(0x1A),mDigitsOffsetY[0],&separator,1);

    char date[11];
    sprintf(date,"%2u/%02u/%04u",rtc.day,rtc.month,rtc.year);
    pCanvas->fillRect(0, 0, 80, 8, 0);
    pFontSmall->drawText(pCanvas,0,0,date);
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

void SmDesktop::drawTime(void)
{
    SmHalRtc::SmHalRtcTime rtc = SmHalRtc::getInstance()->getDateTime();

    static uint8_t newDigits[DIGITS_COUNT];

    newDigits[0] = rtc.hour / 10;
    newDigits[1] = rtc.hour % 10;
    newDigits[2] = rtc.minute / 10;
    newDigits[3] = rtc.minute % 10;
    newDigits[4] = rtc.second / 10;
    newDigits[5] = rtc.second % 10;

    for (uint8_t i = 0; i < DIGITS_COUNT; ++i)
    {
        if (i < 4)
        {
            pFont7SegBig->drawSymbol(pCanvas,mDigitsOffsetX[i],mDigitsOffsetY[i], 0x10 + newDigits[i]);
        }
        else
        {
            pFont7SegSmall->drawSymbol(pCanvas,mDigitsOffsetX[i],mDigitsOffsetY[i], 0x10 + newDigits[i]);
        }
        mDigits[i] = newDigits[i];
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

            menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(), pCanvas);
            menuAnimator.setDirection(SmAnimator::ANIM_DIR_DOWN);
            menuAnimator.setShiftLimit(128);
            menuAnimator.setSpeed(4);
            menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
            menuAnimator.start(0,0,0,0,128,64);

            SmHwKeyboard::getInstance()->subscribe(this);
            SmHalSysTimer::subscribe(this,10,true);
        }
        drawAll();
        drawTime();
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
