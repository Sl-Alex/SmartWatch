#include <cstdio>
#include "sm_notification.h"
#include "sm_display.h"
#include "sm_font.h"
#include "sm_hw_storage.h"

SmNotification::SmNotification(SmHwKeyboardIface * parent)
    :pParent(parent)
{
    pCanvas = SmDisplay::getInstance()->getCanvas();
    mCount = 1;
    drawItem();

    SmHwKeyboard::getInstance()->subscribe(this);
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->allowSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_MENU,
                                            SmHwPowerMgr::SleepTimeout::SM_HW_SLEEP_LONG);
#endif
}

SmNotification::~SmNotification()
{
    SmHwKeyboard::getInstance()->unsubscribe(this);
}

void SmNotification::drawItem(void)
{
    SmImage image;
    SmFont font;

    char tmp[4];

    if (mCount > 99)
        sprintf(tmp, "99+");
    else
        sprintf(tmp, "%d", mCount);

    if (mCount == 1)
        tmp[0] = 0;

    image.init(IDX_ICON_NOTIFICATION);
    font.init(IDX_FW_FONT_SMALL);

    pCanvas->clear();
    pCanvas->drawCanvas(0,0,&image);

    font.drawText(pCanvas, 14, 1, tmp);
    pCanvas->drawHLine(0,127,10,1);

    char tmpHeader[] = "SMS";
    char tmpText[] = "Incoming message";

    font.drawText(pCanvas, 62, 1, tmpHeader);
    font.drawText(pCanvas, 20, 35, tmpText);
}

void SmNotification::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_UP)
    {
    }
    if (key == SM_HW_BUTTON_DOWN)
    {
    }
    if (key == SM_HW_BUTTON_SELECT)
    {
    }
    if (key == SM_HW_BUTTON_EXIT)
    {
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmNotification::onKeyUp(SmHwButtons)
{

}
