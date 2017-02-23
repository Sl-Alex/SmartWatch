#include "sm_notification.h"
#include "sm_display.h"
#include "sm_font.h"
#include "sm_hw_storage.h"

SmNotification::SmNotification(SmHwKeyboardIface * parent)
    :pParent(parent)
{
    pCanvas = new SmCanvas();
    pCanvas->init(128,64);
    drawItem();
    notificationAnimator.setDirection(SmAnimator::ANIM_DIR_UP);
    notificationAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    notificationAnimator.setShiftLimit(128);
    notificationAnimator.setSpeed(4);
    notificationAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
    notificationAnimator.start(0,63,0,0,128,64);

    SmHwKeyboard::getInstance()->subscribe(this);
    SmHalSysTimer::subscribe(this, 10, true);
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->blockSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_MENU);
#endif
}

SmNotification::~SmNotification()
{
    if (notificationAnimator.isRunning())
    {
        notificationAnimator.finish();
        delete pCanvas;
    }

    SmHwKeyboard::getInstance()->unsubscribe(this);
    SmHalSysTimer::unsubscribe(this);
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->allowSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_MENU, 0);
#endif
}

void SmNotification::onTimer(uint32_t)
{
    if (notificationAnimator.isRunning())
    {
        notificationAnimator.tick();
        if (!notificationAnimator.isRunning())
        {
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
        }
    }
}

void SmNotification::drawItem(void)
{
    SmImage image;
    SmFont font;

    image.init(IDX_ICON_NOTIFICATION);
    font.init(IDX_FW_FONT_SMALL);

    pCanvas->clear();
    pCanvas->drawCanvas(48,15,&image);

    char tmp[] = "Notification";

    font.drawText(pCanvas, 32, 45, tmp);
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
        // Finalize animation if it is active
        if (notificationAnimator.isRunning())
        {
            notificationAnimator.finish();
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
        }
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmNotification::onKeyUp(SmHwButtons)
{

}
