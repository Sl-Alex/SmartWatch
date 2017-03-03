#include <cstdio>
#include "sm_notification.h"
#include "sm_display.h"
#include "sm_font.h"
#include "sm_hw_storage.h"

SmNotification::SmNotification(SmHwKeyboardIface * parent, SmText header, SmText text)
    :pParent(parent)
{
    pCanvas = SmDisplay::getInstance()->getCanvas();

    mHeader.push_back(header);
    mText.push_back(text);
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

    sprintf(tmp, "%d", mHeader.size());

    if (mHeader.size() == 1)
        tmp[0] = 0;

    image.init(IDX_ICON_NOTIFICATION);
    font.init(IDX_FW_FONT_SMALL);

    pCanvas->clear();
    pCanvas->drawCanvas(0,0,&image);

    font.drawText(pCanvas, 14, 1, tmp);
    pCanvas->drawHLine(0,127,10,1);

    font.drawText(pCanvas, 62, 1, mHeader.back());
    font.drawText(pCanvas, 20, 35, mText.back());
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
        // Remove current element if any
        if (mHeader.size() > 0)
        {
            SmText header = mHeader.back();
            SmText text = mText.back();
            // Delete allocated data
            delete[] header.pText;
            delete[] text.pText;
            mHeader.pop_back();
            mText.pop_back();
        }
        // Show element if any
        if (mHeader.size() > 0)
        {
            drawItem();
        }
        else
        {
            // Inform parent that we are done
            // Do nothing after this step because we will be destroyed here
            pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
        }
    }
}

void SmNotification::onKeyUp(SmHwButtons)
{

}
