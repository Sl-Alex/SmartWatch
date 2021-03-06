#include "sm_main_menu.h"
#include "sm_display.h"
#include "sm_font.h"
#include "sm_hw_storage.h"

SmMainMenu::SmMainMenu(SmHwKeyboardIface * parent)
    :pParent(parent)
{
    selected = 0;
    pCanvas = new SmCanvas();
    pCanvas->init(128,64);
    drawItem();
    menuAnimator.setDirection(SmAnimator::ANIM_DIR_UP);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(128);
    menuAnimator.setSpeed(4);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
    menuAnimator.start(0,63,0,0,128,64);

    SmHwKeyboard::getInstance()->subscribe(this);
    SmHalSysTimer::subscribe(this, 10, true);

#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->blockSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_MENU);
#endif
}

SmMainMenu::~SmMainMenu()
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.finish();
        delete pCanvas;
    }

    SmHwKeyboard::getInstance()->unsubscribe(this);
    SmHalSysTimer::unsubscribe(this);
#ifndef PC_SOFTWARE
    SmHwPowerMgr::getInstance()->allowSleep(SmHwPowerMgr::SleepBlocker::SM_HW_SLEEPBLOCKER_MENU,
                                            SmHwPowerMgr::SleepTimeout::SM_HW_SLEEP_NONE);
#endif
}

void SmMainMenu::onTimer(uint32_t)
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.tick();
        if (!menuAnimator.isRunning())
        {
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
        }
    }
}

void SmMainMenu::gotoNext(void)
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.finish();
        delete pCanvas;
        pCanvas = SmDisplay::getInstance()->getCanvas();
    }

    pCanvas = new SmCanvas();
    pCanvas->init(128,64);

    if (selected < ITEMS_COUNT - 1)
        selected++;
    else
        selected = 0;

    drawItem();

    menuAnimator.setDirection(SmAnimator::ANIM_DIR_UP);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(0);
    menuAnimator.setSpeed(8);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_SHIFT);
    menuAnimator.start(0,63,0,0,128,64);
}

void SmMainMenu::gotoPrev(void)
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.finish();
        delete pCanvas;
        pCanvas = SmDisplay::getInstance()->getCanvas();
    }

    pCanvas = new SmCanvas();
    pCanvas->init(128,64);

    if (selected > 0)
        selected--;
    else
        selected = ITEMS_COUNT - 1;

    drawItem();

    menuAnimator.setDirection(SmAnimator::ANIM_DIR_DOWN);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(0);
    menuAnimator.setSpeed(8);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_SHIFT);
    menuAnimator.start(0,0,0,0,128,64);
}

void SmMainMenu::drawItem(void)
{
    SmImage image;
    SmFont font;

    image.init(items[selected].imageIndex);
    font.init(IDX_FW_FONT_SMALL);

    pCanvas->clear();
    pCanvas->drawCanvas(16,10,&image);

    SmText text;
    text.pText = items[selected].pText;
    text.length = items[selected].symbolsCount;
    font.drawText(pCanvas, 16, 50, text);
}

void SmMainMenu::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_UP)
    {
        gotoNext();
    }
    if (key == SM_HW_BUTTON_DOWN)
    {
        gotoPrev();
    }
    if (key == SM_HW_BUTTON_SELECT)
    {
        // Finalize animation if it is active
        if (menuAnimator.isRunning())
        {
            menuAnimator.finish();
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
        }
        // Unsubscribe from all notifications
        SmHalSysTimer::unsubscribe(this);
        SmHwKeyboard::getInstance()->unsubscribe(this);
        // Create "Edit" menu
        switch(items[selected].imageIndex)
        {
            case IDX_ICON_BLUETOOTH:
                pEditMenu = new SmEditMenu(this, SmEditMenu::EG_BLUETOOTH);
            break;
            case IDX_ICON_DATE:
                pEditMenu = new SmEditMenu(this, SmEditMenu::EG_DATE);
            break;
            case IDX_ICON_TIME:
                pEditMenu = new SmEditMenu(this, SmEditMenu::EG_TIME);
            break;
            default:
                pEditMenu = nullptr;
        }

    }
    if (key == SM_HW_BUTTON_VIRT_EXIT)
    {
        if (pEditMenu != nullptr)
            delete pEditMenu;

        pEditMenu = nullptr;

        pCanvas = new SmCanvas();
        pCanvas->init(128,64);

        drawItem();
        menuAnimator.setDirection(SmAnimator::ANIM_DIR_RIGHT);
        menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
        menuAnimator.setShiftLimit(128);
        menuAnimator.setSpeed(4);
        menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
        menuAnimator.start(0,0,0,0,128,64);

        SmHwKeyboard::getInstance()->subscribe(this);
        SmHalSysTimer::subscribe(this, 10, true);
    }
    if (key == SM_HW_BUTTON_EXIT)
    {
        // Finalize animation if it is active
        if (menuAnimator.isRunning())
        {
            menuAnimator.finish();
            delete pCanvas;
            pCanvas = SmDisplay::getInstance()->getCanvas();
        }
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmMainMenu::onKeyUp(SmHwButtons)
{

}
