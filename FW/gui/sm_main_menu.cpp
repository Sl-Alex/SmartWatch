#include "sm_main_menu.h"
#include "sm_display.h"

SmMainMenu::SmMainMenu(SmHwKeyboardIface * parent)
    :pParent(parent)
{
    pCanvas = SmDisplay::getInstance()->getCanvas();
    menuForeground.init(128,64);
    menuForeground.clear();
    menuForeground.drawRect(16,16,128-16,64-16,1);
    menuAnimator.setDirection(SmAnimator::ANIM_DIR_UP);
    menuAnimator.setDestSource(pCanvas,&menuForeground);
    menuAnimator.setShiftLimit(128);
    menuAnimator.setSpeed(2);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
    menuAnimator.start(0,63,0,0,128,64);

    SmHwKeyboard::getInstance()->subscribe(this);
    SmHalSysTimer::subscribe(this, 10, true);
}

SmMainMenu::~SmMainMenu()
{
    SmHwKeyboard::getInstance()->unsubscribe(this);
    SmHalSysTimer::unsubscribe(this);
}

void SmMainMenu::onTimer(uint32_t)
{
    menuAnimator.tick();
}

void SmMainMenu::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_EXIT)
    {
        // Finalize animation if it is active
        menuAnimator.finish();
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmMainMenu::onKeyUp(SmHwButtons)
{

}
