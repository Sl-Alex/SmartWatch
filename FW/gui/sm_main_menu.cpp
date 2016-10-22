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
    menuAnimator.setSpeed(2);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
    menuAnimator.start(0,63,0,0,128,64);

    SmHwKeyboard::getInstance()->subscribe(this);
    SmHalSysTimer::subscribe(this, 10, true);
}

SmMainMenu::~SmMainMenu()
{
    if (pCanvas != 0)
        delete pCanvas;
    SmHwKeyboard::getInstance()->unsubscribe(this);
    SmHalSysTimer::unsubscribe(this);
}

void SmMainMenu::onTimer(uint32_t)
{
    menuAnimator.tick();
}

void SmMainMenu::gotoNext(void)
{
    if (selected < ITEMS_COUNT - 1)
        selected++;
    else
        selected = 0;

    menuAnimator.finish();

    drawItem();

    menuAnimator.setDirection(SmAnimator::ANIM_DIR_UP);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(0);
    menuAnimator.setSpeed(2);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_SHIFT);
    menuAnimator.start(0,63,0,0,128,64);
}

void SmMainMenu::gotoPrev(void)
{
    if (selected > 0)
        selected--;
    else
        selected = ITEMS_COUNT - 1;

    menuAnimator.finish();

    drawItem();

    menuAnimator.setDirection(SmAnimator::ANIM_DIR_DOWN);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(0);
    menuAnimator.setSpeed(2);
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

    font.drawText(pCanvas, 16, 50, items[selected].pText, items[selected].symbolsCount);
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
    if (key == SM_HW_BUTTON_EXIT)
    {
        // Finalize animation if it is active
        menuAnimator.finish();
        // Cleanup canvas
        delete pCanvas;
        pCanvas = 0;
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmMainMenu::onKeyUp(SmHwButtons)
{

}
