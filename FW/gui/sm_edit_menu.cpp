#include "sm_edit_menu.h"
#include "sm_display.h"
#include "sm_font.h"
#include "sm_hw_storage.h"
#include "sm_strings.h"

SmEditMenu::SmEditMenu(SmHwKeyboardIface * parent)
    :pParent(parent)
{
    mSelected = 0;
    pCanvas = new SmCanvas();
    pCanvas->init(128,64);
    pCanvas->clear();

    std::vector<SmEditor::SmEditorItem> numbers = std::vector<SmEditor::SmEditorItem>{
        {SM_STRING_0,SM_STRING_0_SZ},
        {SM_STRING_1,SM_STRING_1_SZ},
        {SM_STRING_2,SM_STRING_2_SZ},
        {SM_STRING_3,SM_STRING_3_SZ},
        {SM_STRING_4,SM_STRING_4_SZ},
        {SM_STRING_5,SM_STRING_5_SZ},
        {SM_STRING_6,SM_STRING_6_SZ},
        {SM_STRING_7,SM_STRING_7_SZ},
        {SM_STRING_8,SM_STRING_8_SZ},
        {SM_STRING_9,SM_STRING_9_SZ},
    };

    mEditors[0].init(numbers);
    mEditors[1].init(numbers);
    mEditors[2].init(numbers);
    mEditors[3].init(numbers);

    mEditors[0].setFocus();

    drawItems();
    menuAnimator.setDirection(SmAnimator::ANIM_DIR_LEFT);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(128);
    menuAnimator.setSpeed(4);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_VIS_APPEAR);
    menuAnimator.start(127,0,0,0,128,64);

    SmHwKeyboard::getInstance()->subscribe(this);
    SmHalSysTimer::subscribe(this, 10, true);
}

SmEditMenu::~SmEditMenu()
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.finish();
        delete pCanvas;
    }

    SmHwKeyboard::getInstance()->unsubscribe(this);
    SmHalSysTimer::unsubscribe(this);
}

void SmEditMenu::onTimer(uint32_t)
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

void SmEditMenu::gotoNext(void)
{
    if (menuAnimator.isRunning())
    {
        menuAnimator.finish();
        delete pCanvas;
        pCanvas = SmDisplay::getInstance()->getCanvas();
    }

    if (mSelected < ITEMS_COUNT - 1)
        mSelected++;
    else
        mSelected = 0;

    for (uint32_t i = 0; i < ITEMS_COUNT; i++)
    {
        if (i != mSelected)
            mEditors[i].clearFocus();
        else
            mEditors[i].setFocus();
    }

    drawItems();

/*
    menuAnimator.setDirection(SmAnimator::ANIM_DIR_UP);
    menuAnimator.setDestSource(SmDisplay::getInstance()->getCanvas(),pCanvas);
    menuAnimator.setShiftLimit(0);
    menuAnimator.setSpeed(2);
    menuAnimator.setType(SmAnimator::ANIM_TYPE_SHIFT);
    menuAnimator.start(0,63,0,0,128,64);
*/
}

void SmEditMenu::drawItems(void)
{
//    menuAnimator.finish();

    pCanvas->drawCanvas(30,16,&mEditors[0]);
    pCanvas->drawCanvas(50,16,&mEditors[1]);
    pCanvas->drawCanvas(70,16,&mEditors[2]);
    pCanvas->drawCanvas(90,16,&mEditors[3]);

/*
    SmImage image;
    SmFont font;

    image.init(items[selected].imageIndex);
    font.init(IDX_FW_FONT_SMALL);

    pCanvas->clear();
    pCanvas->drawCanvas(16,10,&image);

    font.drawText(pCanvas, 16, 50, items[selected].pText, items[selected].symbolsCount);
*/
}

void SmEditMenu::onKeyDown(SmHwButtons key)
{
    if (key == SM_HW_BUTTON_UP)
    {
        mEditors[mSelected].onKeyDown(key);
        drawItems();
    }
    if (key == SM_HW_BUTTON_DOWN)
    {
        mEditors[mSelected].onKeyDown(key);
        drawItems();
    }
    if (key == SM_HW_BUTTON_SELECT)
    {
        gotoNext();
    }
    if (key == SM_HW_BUTTON_EXIT)
    {
        // Inform parent that we are done
        // Do nothing after this step because we will be destroyed here
        pParent->onKeyDown(SM_HW_BUTTON_VIRT_EXIT);
    }
}

void SmEditMenu::onKeyUp(SmHwButtons)
{

}
